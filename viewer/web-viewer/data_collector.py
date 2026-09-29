# data_collector.py
import asyncio
import os
import copy
import json
import yaml
from pathlib import Path
from multiprocessing import Process, Queue
from base64 import b64encode
from datetime import datetime, timedelta, timezone
from math import sqrt, atan2, degrees, radians, sin, cos
import time
import random
import socket
import gzip
import threading
from geopy.distance import geodesic
from object_info_custom import get_custom_object_kind
from object_info_custom import get_custom_object_size_orientation
from object_info_custom import get_custom_object_size_color

# コンフィグファイル読み込み
def load_config():
    file_path = os.path.join("setting", "tracking.yaml")

    with open(file_path, encoding="utf-8") as f:
        config = yaml.safe_load(f)

    config.setdefault("trafic_dict", {})
    config.setdefault("sensor", {})

    for rsu in config["rsu_point"]:

        file_path = os.path.join("setting", rsu + ".yaml")
        if not os.path.exists(file_path):
            continue
        with open(file_path, encoding="utf-8") as f:
            rsu_yaml = yaml.safe_load(f)
        for key in ("trafic_dict",
                    "sensor"):
            if key not in rsu_yaml:
                continue
            config[key].update(copy.deepcopy(rsu_yaml[key]))
    return config

# アイコンファイル読み込み
def load_icons():
    folder = os.path.join("templates", "images")
    icons = {}
    for file in os.listdir(folder):
        if not file.endswith(".png"):
            continue
        name = os.path.splitext(file)[0]
        with open(os.path.join(folder, file), "rb") as f:
            print(name)
            icons[name] = (
                "data:image/png;base64," +
                b64encode(f.read()).decode("utf-8")
            )
    return icons

# 種別からアイコンをセット
def obj_set_icon(markers, object_type, object_subtype, object_id, information_source_list, lat, lon, min_ttc, config):
    try:
        object_type = int(object_type)
        if 0 <= object_type < len(config["object_kind"]):
            icon_ = config["object_kind"][object_type]
        else:
            icon_ = config["object_kind_default"]
    except (ValueError, TypeError):
        icon_ = config["object_kind_default"]
    # ユーザが定義する種別が入っている場合は、上書きする
    custom_kind = get_custom_object_kind(icon_, object_type, object_subtype, object_id, information_source_list, min_ttc)
    if custom_kind is not None:
        icon_ = custom_kind

    icon_b64 = config["icons"][icon_]
    scale = config.get("coordinate_scale", 10000000)
    center_lat = int(lat)/scale
    center_lon = int(lon)/scale
    #print("[obj_set_icon_on_lat_lon] marker added", object_id, center_lat, center_lon)
    x_size = 30
    y_size = 30
    if "person" in icon_:
        x_size = 45
        y_size = 30
    markers.append({
        "id": str(object_id),
        "lat": center_lat,
        "lng": center_lon,
        "icon": icon_b64,
        "x":x_size,
        "y":y_size,
        "label": ""
    })
    return center_lat, center_lon

# 向き・長さ・幅から構成されるポリゴン情報をアイコンに付与
# 以下、CooL4 API仕様案からの抜粋
#   orientation: 方位角を0.0125度単位で表す (0: 北, 7200: 東, 14400: 南, 21600: 西, 28800: 不明)
#   length, width: 0.01mで表す (1: 0.01m, 65534: 655.34m, 65535: 不明)
def obj_add_polygon_to_icon(polygons, obj_id, orientation_arg, length_arg, width_arg, center_lat, center_lon, information_source_list, config):
    size = []
    color = "#FF0000"
    orientation = None
    length = None
    width = None
    
    try:
        orientation = int(orientation_arg)
        length = int(length_arg)
        width = int(width_arg)
    except (ValueError, TypeError):
        return
    # カスタム関数をコール
    orientation, length, width = get_custom_object_size_orientation(orientation, length, width)
    color = get_custom_object_size_color(obj_id, information_source_list, config)
    if orientation is None or length is None or width is None:
        return
    # API仕様外の値を除外
    # orientation:
    #   0～21600 : 北～西
    #   28800    : 不明
    if not (0 <= orientation <= 21600 or orientation == 28800):
        return
    # length / width:
    #   1～65534 : 有効値
    #   65535    : 不明
    if not (1 <= length <= 65534):
        return
    if not (1 <= width <= 65534):
        return

    bear = orientation * 0.0125
    lat_dist = length * 0.01 * 0.5
    lon_dist = width * 0.01 * 0.5
    distance = sqrt(lat_dist**2 + lon_dist**2)

    for i in range(1,5):
        bearing = degrees(atan2(lon_dist, lat_dist)) % 360 + bear
        if bearing > 360:
            bearing -= 360
        new_point = geodesic(meters=distance).destination((center_lat, center_lon), bearing)
        size.append([round(new_point.latitude,8), round(new_point.longitude,8)])
        if (i % 2 == 0):
            lat_dist = lat_dist * (-1)
        else:
            lon_dist = lon_dist * (-1)
    
    polygons.append({
        "id": "p" + obj_id,
        "coords": size,
        "strokeColor": color,
        "strokeWidth": 1,
        "fillColor": color,
        "fillOpacity": 0.7
    })
    return

# 物標情報のマーカー生成
#  [想定する入力リスト]
#   0: 物標ID, 1: 時刻,  2: 種別, 3: 種別の信頼度, 4: サブ種別, 5: サブ種別の信頼度
#   6: 緯度,   7: 経度   8: 速度, 9: 加速度
#  10: 向き,  11: 長さ, 12: 幅,  13: 情報源のリスト,  14: 最小TTC
def req_obj(markers, polygons, obj, config):
    folder_path = os.path.join("templates", "images")

    for log in obj:
        colms = log.split(",")
        #if colms[0] == "13020240930001303" or colms[0] == "14020240930001303":
        #    continue
        print(colms[0], colms[1])
        # 種別からアイコンを探し、マーカーにプロット
        center_lat, center_lon = obj_set_icon(markers, colms[2], colms[4], colms[0], colms[13], colms[6], colms[7], colms[14], config)

        # 向き・長さ・幅から構成されるポリゴン情報をアイコンに付与
        obj_add_polygon_to_icon(polygons, colms[0], colms[10], colms[11], colms[12], center_lat, center_lon, colms[13], config)
        #print(time_check, object_id, rsu_id, sensor_id, icon_, center_lat, center_lon)
    return
    
def req_free(polygons, free):
    id_ = 0
    for info in free:
        location = []
        info_list = info.split(",")
        rsu_id = int(info_list[-2].replace("[","").replace("]",""))
        lat = int(info_list[3])/10000000
        lot = int(info_list[4])/10000000
        lat_loc = []
        for loc in info_list[6].split("|"):
            rep = loc.replace("[","")
            lat_loc.append(int(rep.replace("]","")))
        lot_loc = []
        for loc in info_list[5].split("|"):
            rep = loc.replace("[","")
            lot_loc.append(int(rep.replace("]","")))

        location.append([lat,lot])

        for i in range(0,len(lat_loc)):
            x = lot_loc[i] / 100
            y = lat_loc[i] / 100 
            distance = sqrt(x**2 + y**2)
            bearing = degrees(atan2(x, y)) % 360
            new_point = geodesic(meters=distance).destination((lat, lot), bearing)

            location.append([round(new_point.latitude,8), round(new_point.longitude,8)])
        location.append(location[0])
        polygons.append({
                    "id": "F" + info_list[-1] + str(id_),
                    "coords": location,
                    "strokeColor": "blue",
                    "strokeWidth": 2,
                    "fillColor": "blue",
                    "fillOpacity": 0.2})
        id_ = id_ + 1

    return 

# 最新の信号情報に更新
def sig_update_trafic_dict_latest(sig, trafic_dict):
    keys = list(trafic_dict.keys())
    for info in sig:
        info_list = info.split(",")
        if info_list[0] in keys:
            if info_list[1] in trafic_dict[info_list[0]].keys():
                trafic_dict[info_list[0]][info_list[1]]["latest"] = info

# 現在の灯色と残秒数をマーカーにセット
def set_current_signal_marker(markers, signal, idkey, now_its, all_flag, config):
    # 灯色1～灯色6までを参照
    max_schedule_list_count = 6
    # 灯色の色, 最小残秒数, 最大残秒数の3項目
    schedule_item_count = 3

    lonlat_list = signal["position"]
    info = signal["latest"]
    if info == "NONE":
        return
    info_list = info.split(",")
    # correction signal color with scheduled time 
    signal_date_its = int(info_list[2])
    its_diff = now_its - signal_date_its
    min_sum = 0
    max_sum = 0
    end_idx = 0
    for i in range(1, max_schedule_list_count, 1):
        end_idx = i
        schedule_idx = i * schedule_item_count
        if len(info_list) < max_schedule_list_count * schedule_item_count:
            break
        display_signal_only = False
        # 灯色1の最小残秒数が不明(65535)の場合、且つ、現実との差が1秒以内の場合は、信号のみ表示するようにセット
        if i == 1 and info_list[schedule_idx + 1] == "65535" and its_diff <= 1000 :
            # 下記、if (max_remaining_time >= 0) の仕様に合わせている
            max_remaining_time = 0
            display_signal_only = True
        # 現在時刻とのズレが大きくなり、灯色1, 2, ...と先を見た時に、不明値 (65535) に達した場合は、表示するのを止める
        if not display_signal_only and info_list[schedule_idx + 2] == "65535":
            signal["latest"] = "NONE"
            break
        max_sum += int(info_list[schedule_idx + 2])
        min_sum += int(info_list[schedule_idx + 1])
        max_remaining_time = max_sum * 100 - its_diff
        min_remaining_time = min_sum * 100 - its_diff
        display_min = max(0, int(min_remaining_time / 1000))
        display_max = int(max_remaining_time / 1000)              
        if (max_remaining_time >= 0):
            icon_name = config["trafic_light_color"][info_list[schedule_idx]].replace(".png", "")
            icon_b64 = config["icons"][icon_name]
            signal_dic = {
                "id": "s" + idkey + signal["id"],
                "lat": lonlat_list[0],
                "lng": lonlat_list[1],
                "icon": icon_b64,
                "x":50,
                "y":30,
                "label": ""
            }
            if not all_flag and not display_signal_only:
                message = '<div style="width: 90px; height: 70px; font-size: 25px;  border:2px solid #000000;color: black;"> Min:{}\n Max:{}</div>'.format(display_min,display_max)
                signal_dic.update({
                    "label": message,
                    "x": 100,
                    "y": 60
                })
            markers.append(signal_dic)
            break
    else:
        signal["latest"] = "NONE"
    print(f'[set_current_signal_marker] INPUT => info_list: {info_list}')
    print(f'[set_current_signal_marker] OUTPUT => now_its: {now_its}, its_diff: {its_diff}, min: {min_remaining_time}, max: {max_remaining_time}, end_idx: {end_idx}')

    
# 信号情報マーカー生成（毎回表示させるため、件数チェックなし）
#  [想定する入力リスト]
#   0: 交差点ID, 1: 信号灯器グループID, 2: 時刻, 3: 灯色1の色, 
#   (オプション) 4: 灯色1の最小残秒数, 5: 灯色2の最大残秒数, 6: 灯色2, ...
def req_signal(markers, sig, trafic_dict, all_flag, config):
    JST = timezone(timedelta(hours=+9), 'JST')
    now_unix = int(float(datetime.now(JST).timestamp()) * 1000)
    now_its = now_unix - 1072915195000
    # 最新の信号情報に更新
    sig_update_trafic_dict_latest(sig, trafic_dict)

    for idkey in trafic_dict.keys():
        for key in trafic_dict[idkey].keys():
            signal = trafic_dict[idkey][key]
            #print(f'[req_signal] idkey: {idkey}, signal_id_list: {key}')
            # 現在の灯色と残秒数をマーカーにセット
            set_current_signal_marker(markers, signal, idkey, now_its, all_flag, config)
    return


def calc_bearing(rsu_lat, rsu_lon, car_lat, car_lon):
    lat1, lon1, lat2, lon2 = map(radians, [rsu_lat, rsu_lon, car_lat, car_lon])
    delta_lon = lon2 - lon1

    x = sin(delta_lon) * cos(lat2)
    y = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(delta_lon)

    initial_bearing = degrees(atan2(x, y))
    return (initial_bearing + 360) % 360

# 距離の近いRSUリストを得る
def get_nearby_rsus(cross, position, tracking_dic, config):
    car_rsu_config = config.get("car_rsu", {})
    distance_to_rsu = car_rsu_config.get("distance_to_rsu", 190)
    min_bearing = car_rsu_config.get("min_bearing", 45)
    max_bearing = car_rsu_config.get("max_bearing", 225)
    routes = car_rsu_config.get("route", [])
    target_rsus = car_rsu_config.get(cross, {}).get("rsus")

    all_rsu_point = config["rsu_point"]
    # carから始まるものを除外
    rsu_point = {
        name: point
        for name, point in all_rsu_point.items()
        if not name.startswith("car")
    }
    # 対象となるRSUが設定されていれば、対象のみに絞り込む
    if target_rsus is not None:
        rsu_point = {
            name: point
            for name, point in rsu_point.items()
            if name in target_rsus
        }
    
    rsu_list = []
    rsus = []
    if cross not in tracking_dic:
        tracking_dic[cross] = {
            "closest": "",
            "entry_bear": 0,
            "outward_trip": True
        }
    for key in rsu_point.keys():
        try:
            distance_m = geodesic(rsu_point[key], position).m
            print(f"get_nearby_rsus: index: {key}, distance: {distance_m}")
            if distance_to_rsu > distance_m:
                rsu_list.append((distance_m, key))
                bearing = calc_bearing(rsu_point[key][0], rsu_point[key][1], position[0], position[1])
                if bearing < min_bearing or bearing > max_bearing:
                    tracking_dic[cross]["outward_trip"] = True
                else:
                    tracking_dic[cross]["outward_trip"] = False
                tracking_dic[cross]["closest"] = key
                tracking_dic[cross]["entry_bear"] = bearing
        except Exception as e:
            print(f"get_nearby_rsus: failed: rsu={key}, position={position}, error={e}")
            continue
    # 距離の近い順
    rsu_list.sort(key=lambda x: x[0])

    # RSU名だけにする
    rsus = [key for distance_m, key in rsu_list]

    # 前回の最寄りRSUがあり、経路設定がある場合
    if tracking_dic[cross]["closest"] != "" and len(routes) > 0:
        min_distance = car_rsu_config.get("min_distance", 45)
        min_bearing_diff = car_rsu_config.get("min_bearing_diff", 70)
        distance_to_next_rsu = car_rsu_config.get("distance_to_next_rsu", distance_to_rsu)
        set_rsu_if_busstop_nearby(cross, position, tracking_dic, rsu_point, rsus, routes, min_bearing, max_bearing, min_distance, min_bearing_diff, distance_to_rsu)
    
    print(f"get_nearby_rsus return rsus: {rsus}")
    return rsus

def set_rsu_if_busstop_nearby(cross, position, tracking_dic, rsu_point, rsus, routes, min_bearing, max_bearing, min_distance, min_bearing_diff, distance_to_rsu):
    closest = tracking_dic[cross]["closest"]
    bearing = calc_bearing(rsu_point[closest][0], rsu_point[closest][1], position[0], position[1])

    bearing_diff = abs(tracking_dic[cross]["entry_bear"] - bearing)
    if bearing_diff > 180:
        bearing_diff = 360 - bearing_diff
    distance_m = geodesic(rsu_point[closest], position).m
    print(f"get_nearby_rsus: bearing_diff: {bearing_diff}, bearing: {bearing}, distance_m: {distance_m}")
    if bearing_diff > min_bearing_diff and distance_m >= min_distance:
        index = stops.index(closest)
        next_stop = ""
        if bearing < min_bearing or bearing > max_bearing:
            next_stop = stops[index - 1]
        else:
            next_stop = stops[index + 1]
        
        if next_stop != "":
            next_distance_m = geodesic(rsu_point[next_stop], position).m
            if distance_m > next_distance_m:
                tracking_dic[cross]["closest"] = next_stop
                tracking_dic[cross]["entry_bear"] = calc_bearing(rsu_point[next_stop][0], rsu_point[next_stop][1], position[0], position[1])
        else:
            if distance_m > distance_to_rsu:
                tracking_dic[cross]["closest"] = ""
        closest = next_stop
        if closest != "":
            if closest in rsus:
                rsus.remove(closest)
                rsus.insert(0, closest)
    else:
        print("get_nearby_rsus - false")
    
    return 


# センサーの検知範囲をセット
def set_sensor_detection_range(rsu_idmap, rsu, fg, config):
    if rsu not in rsu_idmap:
        return
    target_rsuid_list = rsu_idmap[rsu]
    if "sensor" in config.keys():
        for rsu_id in config["sensor"].keys():
            if rsu_id in target_rsuid_list:
                for sensor_id in config["sensor"][rsu_id].keys():
                    location = []
                    lat = config["sensor"][rsu_id][sensor_id]["pos"][0]/10000000
                    lot = config["sensor"][rsu_id][sensor_id]["pos"][1]/10000000
                    lat_loc = (config["sensor"][rsu_id][sensor_id]["area_lat"])
                    lot_loc = (config["sensor"][rsu_id][sensor_id]["area_lon"])
                    for i in range(0,len(lat_loc)):
                        x = lot_loc[i] / 100
                        y = lat_loc[i] / 100 
                        distance = sqrt(x**2 + y**2)
                        bearing = degrees(atan2(x, y)) % 360
                        new_point = geodesic(meters=distance).destination((lat, lot), bearing)

                        location.append([round(new_point.latitude,8), round(new_point.longitude,8)])
                    location.append(location[0])
                    fg.append({
                        "id": "s" + rsu_id + sensor_id,
                        "coords": location,
                        "strokeColor": config["sensor"][rsu_id][sensor_id]["color"],
                        "strokeWidth": 2,
                        "fillColor": config["sensor"][rsu_id][sensor_id]["color"],
                        "fillOpacity": 0.1
                    })
    return

# 自車の位置と、RSUの選択
def get_car_pos_and_rsu(car_pos_dict, markers, logs, cross, tracking_dic, config):
    do_update = False
    rsus = []
    if cross.startswith("car"):
        if cross in logs:
            if "obj" in logs[cross]:
                obj = logs[cross]["obj"]
                if len(obj) > 0:
                    # sort済みなので最後の1件だけ取得
                    log = obj[-1]
                    col = log.split(",")
                    car_pos_dict[cross] = ((int(col[6])/10000000), (int(col[7])/10000000))
                    icon_b64 = config["icons"]["car_own"]
                    markers.append({
                            "id": obj[0], "icon": icon_b64, "x":30, "y":30, "label": "",
                            "lat": car_pos_dict[cross][0],
                            "lng": car_pos_dict[cross][1]
                        })
                    print(f"update car_pos: {car_pos_dict[cross][0]}, {car_pos_dict[cross][1]}")
                    do_update = True
    if cross in car_pos_dict:
        # UDPデータの自車の位置が含まれている場合は、最寄りのRSU抽出
        if do_update:
            rsus = get_nearby_rsus(cross, car_pos_dict[cross], tracking_dic, config)
        else:
            closest = tracking_dic[cross]["closest"]
            rsus = [closest] if closest != "" else []
    else:
        # ビューア（app.py）上で選択されたRSUの名前をセット
        rsus.append(cross.replace("all","").replace("position",""))
    return rsus

# マーカー生成処理
def request_marker(car_pos_dict, logs, cross, signal_save_dic, tracking_dic, config):
    if len(list(logs.keys())) == 0:
        return "",[]
    
    markers = []
    polygons = []

    all_flag = False
    router_info= ""
    obj = []
    sig = []
    free = []

    # 自車の位置と、RSUの選択
    rsus = get_car_pos_and_rsu(car_pos_dict, markers, logs, cross, tracking_dic, config)
    if len(rsus) == 0:
        # 広域視点で描画するように、自車位置と信号をセット
        for key in logs.keys():
            if key == "pos" or key == "router":
                continue
            if "sig" in logs[key]:
                for s in tuple(logs[key]["sig"]):
                    sig.append(s)
            all_flag = True
    else:
        for rsu in rsus:
            if rsu not in logs:
                print(f"[request_marker] Warning: key is not found in logs. key: {rsu}")
                continue
            # センサーの検知範囲をセット
            set_sensor_detection_range(config["rsu_idmap"], rsu, polygons, config)
            if "obj" in logs[rsu]:
                obj.extend(logs[rsu]["obj"])
            if "sig" in logs[rsu]: 
                sig.extend(logs[rsu]["sig"])
            if "free" in logs[rsu]: 
                free.extend(logs[rsu]["free"])

    # 信号情報マーカー処理（毎回表示させるため、件数チェックなし）
    req_signal(markers, tuple(sig), signal_save_dic, all_flag, config)

    if len(free) > 0:
        # フリースペース情報のマーカー生成
        req_free(polygons, tuple(free))    
    logs.clear()
    if len(obj) > 0 and not all_flag:
        # 物標情報のマーカー生成
        req_obj(markers, polygons, tuple(obj), config)

    return rsus, tuple(markers), tuple(polygons)

def get_center_point(car_pos_dict, cross, rsus, config):
    center_point = config["map_center"]
    if cross.startswith("car"):
        if cross in car_pos_dict:
            center_point = car_pos_dict[cross]
            return center_point
    if len(rsus) > 0:
        rsu = rsus[0]
        if rsu in config["rsu_point"]:
            center = config["rsu_point"][rsu]
    return center_point
# UDP 受信でマーカー情報を取得し Queue に送信
# 受信データは JSON 形式を想定：
# [{"id":"m1","lat":35.001,"lng":135.001,"icon_idx":2}, ...]
def udp_collector(queue: Queue, config, host="0.0.0.0", port=33333):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((host, port))
    cross = ""
    car_pos_dict = {}
    print(f"[UDPCollector] Listening on {host}:{port}")

    signal_save_dic = copy.deepcopy(config["trafic_dict"])

    for id_key in signal_save_dic.keys():
        for key in signal_save_dic[id_key].keys():
            signal_save_dic[id_key][key]["latest"] = "NONE"
    tracking_dic = {}

    counter = 0
    while True:
        start = time.time()
        counter = counter + 1
        data, addr = sock.recvfrom(4096)
        logs_json = (gzip.decompress(data)).decode("utf-8")
        logs = json.loads(logs_json)
        print(logs)
        # app.pyから交差点情報を受け取った場合
        if "cross" in logs.keys():
            cross = logs["cross"]
            continue
        elif cross == "":
            # WebSocketクライアントが立ち上がっていない場合
            print(logs)
            continue
        
        ### マーカー、ポリゴン生成
        rsus, markers, polygons = request_marker(car_pos_dict, logs, cross, signal_save_dic, tracking_dic, config)
        # 地図の中心位置を得る
        center_point = get_center_point(car_pos_dict, cross, rsus, config)
        if len(markers) > 0:
            print("[udp_collector] rsu:", rsus, ", markers: ", len(markers), ", center_point: ", center_point[0], ",", center_point[1])
        message = json.dumps({
            "center": {"lat": center_point[0], "lng": center_point[1]},
            "markers": markers,
            "polygons": polygons,
            "created_at_unix":int(time.time())
        })
        queue.put(message)
        if counter % 100 == 0:
            end = time.time()
            counter = 0
            print("avg analyzed_time: {:.6f} sec".format((end - start) / 100))

def start_collectors(queue: Queue):

    config = load_config()
    config["icons"] = load_icons()
    # UDP 受信プロセス
    p2 = Process(target=udp_collector, args=(queue, config))
    p2.daemon = True
    p2.start()

    return [p2]

if __name__ == "__main__":
    q = Queue()
    start_collectors(q)
    import time
    while True:
        if not q.empty():
            msg = q.get()
        time.sleep(0.1)
