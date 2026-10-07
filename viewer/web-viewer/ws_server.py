import asyncio
import argparse
import json
from multiprocessing import Queue
import websockets
from data_collector import start_collectors

async def websocket_handler(websocket, path, queue: Queue):
    latest_data = None
    try:
        while True:
            while not queue.empty():
                latest_data = queue.get_nowait()
            if latest_data:
                await websocket.send(latest_data)
            await asyncio.sleep(0.1)
    except websockets.exceptions.ConnectionClosed:
        print("WebSocket connection closed when radio button changed")

async def main(coordinate_scale=None):
    queue = Queue()
    start_collectors(queue, coordinate_scale)

    print("Starting WebSocket server on ws://0.0.0.0:8765")
    server = await websockets.serve(lambda ws, path: websocket_handler(ws, path, queue),
                                    "0.0.0.0", 8765)
    await server.wait_closed()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--coordinate-scale",
        type=int,
        default=None,
        help="Coordinate scale. Overrides the value in the configuration file."
    )
    args = parser.parse_args()

    asyncio.run(main(args.coordinate_scale))
