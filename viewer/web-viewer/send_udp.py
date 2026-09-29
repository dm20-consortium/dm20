import socket
import pickle
import threading
import subprocess
import time
import gzip
import yaml
import os
import json
from datetime import datetime, timedelta, timezone

processed_obj = set()
latest_time = None
TIME_ROLLBACK_THRESHOLD = 10000   # 10秒以上過去へ戻ったら新しいリプレイと判断

# yamlファイルロード
def load_yaml():
    file_path = os.path.join("setting", "tracking.yaml")
    with open(file_path, encoding='utf-8')as f:
        yaml_doc = yaml.safe_load(f)
    return yaml_doc

# 情報別に辞書リストへセット
def make_info_dict(logs):
    rec_log = False
    rec_title = ""
    rec_list = []
    rec_dict = {}

    ### 下記の行構成で届く事を前提に、情報別に辞書リストへ格納する。
    # *** rec_obj: information_source_list***"
    # 物標情報
    # *** rec_sig: crp_id ***"
    # 信号情報
    # *** rec_free: freespace ***"
    # フリースペース情報
    for log in logs:
        if "rec_obj" in log:
            rec_log = True
        if rec_log:
            if len(log) == 0:
                continue
            elif "***" in log:
                if len(rec_list) != 0:
                    # 情報切替え（例：rec_obj から rec_sig）時に、辞書リストへセット
                    rec_dict[rec_title] = rec_list
                    rec_list = []
                # タイトル設定
                rec_title = log.replace("*** ","").split(":")[0]
            else:
                rec_list.append(log)
    if len(rec_list) != 0:
        # 最後の情報（rec_free）を辞書リストへセット
        rec_dict[rec_title] = rec_list
    return rec_dict
    
# メッセージ作成
def make_message(rec_dict, config):
    global latest_time
    # 物標情報の情報源のリストから、交差点名を得るリスト
    rsu_sid_map = {}
    for name, sid_list in config["rsu_idmap"].items():
        for sid in sid_list:
            rsu_sid_map[f"[{sid}]"] = name
    
    # 信号情報の交差点IDから交差点名を得るリスト
    sig_sid_map = {
        sid: name
        for name, sid in config["sig_idmap"].items()
    }

    #print(f"[make_message] rec_dict:{rec_dict}")

    area_names = set(config.get("rsu_idmap", {}).keys()) | \
                set(config.get("sig_idmap", {}).keys())
    area_data = {}
    for name in sorted(area_names):
        area_data[name] = {
            "obj": [],
            "sig": [],
            "free": []
        }
    # その他
    area_data["other"] = {
        "obj": [],
        "sig": [],
        "free": []
    }
    if "rec_obj" in rec_dict.keys():
        for line in rec_dict["rec_obj"]:
            col = line.split(",")
            if len(col) < 14:
                print(f"[make_message] input error:{line}")
                continue
            print(f"[make_message] rec_obj:{col}")
            '''
            current_time = int(float(col[1]))
            if latest_time is not None and current_time < latest_time - TIME_ROLLBACK_THRESHOLD:
                print(f"[make_message] replay time reset detected: {current_time} < {latest_time}")
                processed_obj.clear()
                latest_time = None
            '''
            obj_key = (col[0], col[1])
            # 同じ objid + time はスキップ
            if obj_key in processed_obj:
                continue
            processed_obj.add(obj_key)

            #if latest_time is None or current_time > latest_time:
            #    latest_time = current_time
            
            rsu_id = col[13]
            ###
            line = ",".join(col)
            if len(col) <= 14:
                line = line + ",-1.0"
            area = rsu_sid_map.get(rsu_id, "other")
            area_data[area]["obj"].append(line)
    for name in sorted(area_names):
        if len(area_data[name]["obj"]) > 0:
            print(f'{name}: {area_data[name]["obj"]}')

    seen_sig = set()
    if "rec_sig" in rec_dict.keys():
        for line in rec_dict["rec_sig"]:
            sid,signal,_= line.split(",",2)
            area = sig_sid_map.get(sid, "other")
            key = (sid, signal)
            if key in seen_sig:
                continue
            seen_sig.add(key)
            area_data[area]["sig"].append(line)

    if "rec_free" in rec_dict.keys():
        free_id = ""
        for line in rec_dict["rec_free"]:
            col = line.split(",")
            rsu_id = col[-2]
            area = rsu_sid_map.get(rsu_id, "other")
            area_data[area]["free"].append(line)


    #print(rec_dict)
    # タイムゾーンの生成
    JST = timezone(timedelta(hours=+9), 'JST')
    rec_dict["time"] = datetime.now(JST).strftime("%Y/%m/%d %H:%M:%S")
    #sig_message = bytes(json.dumps(rec_dict), 'utf-8')
    message = bytes(json.dumps({
        **area_data
        }), 'utf-8')
    #print(f"[make_message] result:{message}")
    return message

### メイン処理
if __name__ == "__main__":
    # udpによるメッセージ送信先
    server_address = "localhost"
    server_port = 33333
    sh_log_command = "sh ./script/check_for_master.sh"
    yaml_doc = load_yaml()

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    while True:
        # コマンドの実行
        sh_log_result = subprocess.run(sh_log_command, stdout=subprocess.PIPE, shell=True).stdout
        logs = sh_log_result.decode("utf8").split("\n")

        if len(logs) == 0:
            print(f"[main] Warning ... no input by command [{sh_log_command}]")
            time.sleep(0.25)
            continue
        # 情報別に辞書リストへセット
        rec_dict = make_info_dict(logs)
        if len(rec_dict) == 0:
            print(f"[main] Warning ... no obj/free/sig input by logs [{logs}]")
            time.sleep(0.25)
            continue

        # コマンドの実行結果から、送信用メッセージ作成
        message = make_message(rec_dict, yaml_doc)
        # gzipによる圧縮
        compressed_message = gzip.compress(message)
        length = len(compressed_message)
        print(f"[main] send to {server_address}:{server_port}, size = {length}")
        # 圧縮データをサーバへ送信
        client_socket.sendto(compressed_message, (server_address, server_port))
        time.sleep(0.25)
        #time.sleep(0.05)
