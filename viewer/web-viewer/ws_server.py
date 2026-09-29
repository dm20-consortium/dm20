import asyncio
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

async def main():
    queue = Queue()
    start_collectors(queue)

    print("Starting WebSocket server on ws://0.0.0.0:8765")
    server = await websockets.serve(lambda ws, path: websocket_handler(ws, path, queue),
                                    "0.0.0.0", 8765)
    await server.wait_closed()

if __name__ == "__main__":
    asyncio.run(main())