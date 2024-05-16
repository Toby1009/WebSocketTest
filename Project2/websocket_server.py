import asyncio
import websockets

async def handle_connection(websocket, path):
    print(f"Client connected from {websocket.remote_address}")
    try:
        async for message in websocket:
            print(f"Received message from client: {message}")
            ##response = f"Received message: {message}"
            await websocket.send(message)
    except websockets.exceptions.ConnectionClosed as e:
        print(f"Client disconnected: {e}")

async def main():
    async with websockets.serve(handle_connection, "localhost", 8765):
        print("WebSocket server started...")
        await asyncio.Future()  # run forever

asyncio.run(main())
