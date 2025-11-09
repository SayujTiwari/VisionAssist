import asyncio
import threading
from flask import Flask, send_from_directory
from flask_socketio import SocketIO
from bleak import BleakClient, BleakScanner

# === BLE Identifiers (match Arduino) ===
DEVICE_NAME = "VisonAssist"
SERVICE_UUID = "c1d0a000-1234-4abc-bbbb-1234567890ab"
CHAR_UUID = "c1d0a001-1234-4abc-bbbb-1234567890ab"

app = Flask(__name__, static_folder="static")
socketio = SocketIO(app, cors_allowed_origins="*", async_mode="threading")

# === Bluetooth Logic ===
async def connect_ble():
    print("Scanning for VisionAssist...")
    device = None
    devices = await BleakScanner.discover(timeout=5)
    for d in devices:
        if DEVICE_NAME in d.name:
            device = d
            break
    if not device:
        print("Device not found.")
        await asyncio.sleep(5)
        return await connect_ble()

    print(f"Connecting to {device.name} ({device.address})...")
    async with BleakClient(device.address) as client:
        print("Connected:", client.is_connected)

        def handle(_, data):
            try:
                json_str = data.decode("utf-8")
                socketio.emit("distance", {"data": json_str})
                print(json_str)
            except Exception as e:
                print("Decode error:", e)

        await client.start_notify(CHAR_UUID, handle)
        while True:
            await asyncio.sleep(1)

def ble_thread():
    asyncio.run(connect_ble())

threading.Thread(target=ble_thread, daemon=True).start()

# === Flask Routes ===
@app.route("/sensorInfo")
def index():
    return send_from_directory("static", "index.html")

if __name__ == "__main__":
    socketio.run(app, host="0.0.0.0", port=3000)


