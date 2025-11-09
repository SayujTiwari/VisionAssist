import asyncio
from bleak import BleakScanner, BleakClient

DEVICE_NAME = "VisionAssist"
SERVICE_UUID = "c1d0a000-1234-4abc-bbbb-1234567890ab"
CHAR_UUID = "c1d0a001-1234-4abc-bbbb-1234567890ab"

async def main():
    print("Scanning for VisionAssist...")
    device = None
    devices = await BleakScanner.discover(timeout=5)
    for d in devices:
        if d.name == DEVICE_NAME:
            device = d
            break

    if not device:
        print("Device not found. Make sure VisionAssist is advertising.")
        return

    print(f"Found device: {device.name} [{device.address}]")

    async with BleakClient(device.address) as client:
        print("Connected:", client.is_connected)

        def handle(_, data: bytearray):
            try:
                msg = data.decode("utf-8").strip()
                print(msg)
            except Exception as e:
                print("Decode error:", e)

        await client.start_notify(CHAR_UUID, handle)
        print("Listening for data... (Ctrl+C to exit)")

        try:
            while True:
                await asyncio.sleep(1)
        except KeyboardInterrupt:
            print("\nStopping notifications...")
            await client.stop_notify(CHAR_UUID)

if __name__ == "__main__":
    asyncio.run(main())
