# 物標IDからセンサーIDを求める（CooL4 API 仕様書に基づく）
def get_sensor_id(object_id_column):
    sensor_id = -1
    try:
        object_id = int(object_id_column.replace("^C", ""))
        rsu_id = object_id & 0xffffffff
        sensor_id = (object_id >> 48) & 0xff
    except Exception as e:
        print(f"[req_obj] Object ID {object_id_column} Specification Compliance Error : {e}")
    return rsu_id, sensor_id

# 物標種別をカスタマイズする
#   セットできる文字列）
#     question, car, train, motorcycle, bicycle, person, dog, trailer, flag
def get_custom_object_kind(icon_, object_type, object_subtype, object_id, information_source_list, min_ttc):
    # TTCが低い場合、色を赤くする
    try:
        min_ttc_value = float(min_ttc)
        if min_ttc_value > 0:
            icon_ = icon_ + "_red"
            print(f"danger - object_id: {object_id}, min_ttc: {min_ttc}")
            return icon_
        else:
            return None
    except ValueError:
        return None
    
    print(f"get_custom_object_kind ttc{min_ttc}")
    return None
    # 一律、車両とする場合の例
    return "car"
    rsu_id, sensor_id = get_sensor_id(object_id)
    icon_ = None
    if sensor_id == -1:
        return None
    if rsu_id == 302120965:
        icon_ = "car"
    elif rsu_id == 302120961:
        if sensor_id == 1:
            icon_ = "car"
        else:
            icon_ = "person"
    else:
        return None
    return icon_

# 向き・長さ・幅をCooL4 API仕様案に合わせて変換する
# 以下、CooL4 API仕様案からの抜粋
#   orientation: 方位角を0.0125度単位で表す (0: 北, 7200: 東, 14400: 南, 21600: 西, 28800: 不明)
#   length, width: 0.01mで表す (1: 0.01m, 65534: 655.34m, 65535: 不明)
def get_custom_object_size_orientation(orientation, length, width):
    # 物標がサイズ情報を持たない場合
    return None, None, None
    # 変換不要の場合
    return orientation, length, width

    
# 物標サイズ描画時の色を指定する
def get_custom_object_size_color(object_id, information_source_list, config):
    #一律、同じ色を返す場合
    return "#FF0000"

    color = "#FF0000"

    rsu_id, sensor_id = get_sensor_id(object_id)
    # 下記は、yaml情報に含まれている, sensor毎のcolor を元にセットする例
    if sensor_id != -1:
        if str(sensor_id) in config["sensor"][str(rsu_id)][str(sensor_id)]:
            color = config["sensor"][str(rsu_id)][str(sensor_id)]["color"]

    return color
