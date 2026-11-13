# app.py
import streamlit as st
import streamlit.components.v1 as components
import const
import socket
import gzip
import json
import os
import yaml
import argparse
import sys

parser = argparse.ArgumentParser()
#タイルサーバと著作権表示
parser.add_argument("--tile-url", default="", help="Leaflet tile URL")
parser.add_argument("--attribution", default="", help="Map tile attribution")
parser.add_argument("--tile-max-zoom", default="", help="Leaflet tile Max Zoom")
#グリッドマップの表示（任意）
parser.add_argument("--gridmap-url", default="", help="Lanelet2 GridMap vector tile URL")

args = parser.parse_args(sys.argv[1:])

if not args.tile_url:
    parser.error("--tile-url is required")
if not args.attribution:
    parser.error("--attribution is required")
tile_max_zoom = ""
if args.tile_max_zoom:
    tile_max_zoom = f", maxZoom: {args.tile_max_zoom}"

tile_url = args.tile_url
attribution = args.attribution
tile_url_js = json.dumps(tile_url)
attribution_js = json.dumps(attribution)
gridmap_url = args.gridmap_url
gridmap_js = ""

if gridmap_url:
    gridmap_js = f"""
L.vectorGrid.protobuf(
    {json.dumps(gridmap_url)},
    {{
        vectorTileLayerStyles: {{
            "A": {{
                fill: true,
                fillColor: "#696969",
                color: "transparent"
            }},
            "S": {{
                color: "#000000",
                weight: 1,
                opacity: 0.5
            }}
        }}
    }}
).addTo(map);
"""

st.set_page_config(**const.SET_PAGE_CONFIG)
st.markdown("""
<style>
header.stAppHeader {
    background-color: transparent;
}
section.stMain .block-container {
    padding-top: 0rem;
    z-index: 1;
    background-color: gray;
}
footer.stAppFooter {
    background-color: transparent;
}
</style>
""", unsafe_allow_html=True)
#st.set_page_config(layout="wide")
st.header("DM Web Viewer", divider="rainbow")

file_path = os.path.join("conf", "web-viewer.yaml")

with open(file_path, encoding="utf-8") as f:
    config = yaml.safe_load(f)
rsu_point = config["rsu_point"]
zoom = config.get("map_zoom", 16)
rsu_zoom = config.get("rsu_zoom", {})

# radioの選択肢
choices = list(rsu_point.keys())
choice = st.radio(label="交差点を選択してください", options=choices, index=0, horizontal=True,)

client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
ws_server_address = "localhost"
ws_server_port = 33333

if "last_choice" not in st.session_state:
    st.session_state.last_choice = None
if choice != st.session_state.last_choice:
    # ファイルの1行目を書き換える
    message = bytes(json.dumps({"cross":choice}), "utf-8")
    client_socket.sendto(gzip.compress(message), (ws_server_address, ws_server_port))
    # 書き込み
    st.session_state.last_choice = choice

rsu_info = rsu_point[choice]
center_lat = rsu_info[0]
center_lon = rsu_info[1]
initial_zoom = rsu_zoom.get(choice, zoom)
    
html = """
<!DOCTYPE html>
<html lang="ja">
<head>
    <meta charset="UTF-8">
    <title>Leaflet + WebSocket</title>
    <link rel="stylesheet" href="https://unpkg.com/leaflet/dist/leaflet.css"/>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/Leaflet.awesome-markers/2.0.2/leaflet.awesome-markers.css"/>
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css">

    <style>
        body { margin: 0; padding: 0; }
        #map { width: 100%; height: 100vh; }
        /* 信号マーカー */
        .signal-marker {
            background: transparent;
            border: none;
        }

        /* 信号画像とラベルのコンテナ */
        .signal-container {
            position: relative;
        }

        /* 信号の残秒数ラベル */
        .signal-label {
            position: absolute;
            left: 100%;
            top: 0;
            margin-left: 5px;
            white-space: nowrap;
        }
    </style>
</head>
<body>
    <div id="clock" style="
        position:absolute;
        top:10px;
        left:90%;
        transform:translateX(-10%);
        background:white;
        padding:6px 12px;
        font-size:26px;
        z-index:1000;
    ">---</div>
    <div id="map"></div>
    <script src="https://unpkg.com/leaflet/dist/leaflet.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/Leaflet.awesome-markers/2.0.2/leaflet.awesome-markers.js"></script>
    <script src="https://unpkg.com/leaflet.vectorgrid/dist/Leaflet.VectorGrid.bundled.js"></script>

    <script>
        let contentTimestamp = null;
        const IS_CAR = __IS_CAR__;

        const map = L.map('map').setView([__CENTER_LAT__, __CENTER_LON__], __ZOOM__);
        L.tileLayer(__TILE_URL__, {attribution: __ATTRIBUTION__ __TILE_MAX_ZOOM__}).addTo(map);
        __GRIDMAP_JS__

        const markers = {};
        // ズームレベルに応じた信号アイコンの倍率
        function getSignalScale(zoom) {
            return Math.max(0.5, Math.min(2.0, 1 + (zoom - 20) * 0.25));
        }

        // 信号アイコンとラベルを一体化
        function createSignalIcon(m) {
            const zoom = map.getZoom();
            const scale = getSignalScale(zoom);

            // Zoomレベルに応じてサイズ変更
            const width = Math.round((m.x || 100) * scale);
            const height = Math.round((m.y || 60) * scale);

            // Zoom 19以上でラベル表示
            const labelHtml = (m.label && zoom >= 19)
                ? `<div class="signal-label">${m.label}</div>`
                : "";

            const html = `
                <div class="signal-container">
                    <img src="${m.icon}"
                        style="display:block; width:${width}px; height:${height}px;">
                    ${labelHtml}
                </div>
            `;

            return L.divIcon({
                html: html,
                className: "signal-marker",
                iconSize: [width, height],
                iconAnchor: [width / 2, height / 2]
            });
        }

        const polygonMap = {};

        const ws = new WebSocket("ws://localhost:8765");
        var busMarker = L.AwesomeMarkers.icon({
                icon: 'bus-simple',
                markerColor: 'green'
            });

        ws.onmessage = function(event) {
            const data = JSON.parse(event.data);
            contentTimestamp = data.created_at_unix * 1000;
            if (contentTimestamp) {
                const displayTime = new Date(contentTimestamp);
                document.getElementById("clock").innerText = "生成時刻" + displayTime.toLocaleTimeString();
            }
            // 車両の場合は、位置追従
            if (data.center) {
                if (IS_CAR) {
                    map.setView([data.center.lat, data.center.lng]);
                }
            }
            // 現在表示中のマーカーID
            const currentIDs = Object.keys(markers);
            // 新しく受信したマーカーID
            const newIDs = data.markers ? data.markers.map(m => m.id) : [];

            // 削除すべきマーカー
            currentIDs.forEach(id => {
                if (!newIDs.includes(id)) {
                    map.removeLayer(markers[id]);
                    delete markers[id];
                }
            });

            if (data.markers) {
                data.markers.forEach(m => {
                    let icon;

                    if (m.icon && m.id.startsWith("s")) {
                        icon = createSignalIcon(m);
                    } else if (m.icon) {
                        icon = L.icon({
                            iconUrl: m.icon,
                            iconSize: [m.x, m.y]
                        });
                    } else {
                        icon = busMarker;
                    }

                    if (markers[m.id]) {
                        markers[m.id].setLatLng([m.lat, m.lng]);
                        markers[m.id].setIcon(icon);
                    } else {
                        markers[m.id] = L.marker([m.lat, m.lng], {
                            icon: icon
                        }).addTo(map);
                    }
                });
            }
            if (data.polygons) {
                const receivedIDs = data.polygons.map(p => p.id);
                const existingIDs = Object.keys(polygonMap);

                // 削除すべきPolygon
                existingIDs.forEach(id => {
                    if (!receivedIDs.includes(id)) {
                        map.removeLayer(polygonMap[id]);
                        delete polygonMap[id];
                    }
                });

                // 新規・更新 Polygon
                data.polygons.forEach(poly => {
                    // 既存 → 更新
                    if (polygonMap[poly.id]) {
                        polygonMap[poly.id].setLatLngs(poly.coords);
                        polygonMap[poly.id].setStyle({
                            color: poly.strokeColor,
                            weight: poly.strokeWidth,
                            fillColor: poly.fillColor,
                            fillOpacity: poly.fillOpacity
                        });
                    } else {
                        // 新規作成
                        const polygon = L.polygon(poly.coords, {
                            color: poly.strokeColor,
                            weight: poly.strokeWidth,
                            fillColor: poly.fillColor,
                            fillOpacity: poly.fillOpacity
                        }).addTo(map);
                        polygonMap[poly.id] = polygon;
                    }
                });
            }
        };
        console.log(data);
        console.log(data.markers);
    </script>
</body>
</html>
"""
is_car = choice.startswith("car")

html = html.replace("__CENTER_LAT__", str(center_lat))
html = html.replace("__CENTER_LON__", str(center_lon))
html = html.replace("__ZOOM__", str(initial_zoom))
html = html.replace("__IS_CAR__", str(is_car).lower())
html = html.replace("__TILE_URL__", tile_url_js)
html = html.replace("__TILE_MAX_ZOOM__", tile_max_zoom)
html = html.replace("__ATTRIBUTION__", attribution_js)
html = html.replace("__GRIDMAP_JS__", gridmap_js)

components.html(html, height=800)
