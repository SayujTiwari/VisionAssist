// utils/ble.ts
import { emitValue } from "./state";

const NUS_SERVICE = "c1d0a000-1234-4abc-bbbb-1234567890ab";
const NUS_TX = "c1d0a001-1234-4abc-bbbb-1234567890ab"; // notify device → browser

export async function connectBLE() {
  try {
    console.log("Requesting Bluetooth device...");
    const device = await navigator.bluetooth.requestDevice({
      filters: [{ services: [NUS_SERVICE] }],
      optionalServices: [NUS_SERVICE],
    });

    const server = await device.gatt?.connect();
    if (!server) throw new Error("Failed to connect GATT");
    const service = await server.getPrimaryService(NUS_SERVICE);
    const txChar = await service.getCharacteristic(NUS_TX);

    // Handle incoming data from Arduino
    txChar.addEventListener("characteristicvaluechanged", (event: Event) => {
    const target = event.target as BluetoothRemoteGATTCharacteristic;
    const val = target.value;
    if (!val) return;

    const value = new TextDecoder().decode(val);
    try {
        const parsed = JSON.parse(value);
        emitValue(parsed);
    } catch {
        console.warn("Invalid BLE data:", value);
    }
    });

    await txChar.startNotifications();
    console.log("Connected and listening to BLE notifications");
  } catch (err) {
    console.error("BLE connection failed:", err);
    console.log("Running in simulation mode...");
    simulateData(); // fallback if no device
  }
}

function simulateData() {
  setInterval(() => {
    const data = {
      front: Math.random() * 2,
      left: Math.random() * 2,
      right: Math.random() * 2,
      msg: Math.random() < 0.3 ? "stop" : "clear",
    };
    emitValue(data);
  }, 1000);
}
