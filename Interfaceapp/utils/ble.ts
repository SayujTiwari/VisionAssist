import { updateGuidance, currentGuidance } from "@/utils/guidence"
import { setValue } from "./state";

const ARDUINO_NAME = 'VisionAssist'
const NUS_SERVICE = 'c1d0a000-1234-4abc-bbbb-1234567890ab';
const NUS_RX = '6e400002-b5a3-f393-e0a9-e50e24dcca9e'; // write from browser → device
const NUS_TX = 'c1d0a001-1234-4abc-bbbb-1234567890ab'; // notify device → browser

let gDevice: BluetoothDevice | null = null;
let gServer: BluetoothRemoteGATTServer | null = null;
let rxChar: BluetoothRemoteGATTCharacteristic | null = null;
let txChar: BluetoothRemoteGATTCharacteristic | null = null;

interface bleData {
    left: number;
    right: number;
}

export async function connectBLE() {
    gDevice = await navigator.bluetooth.requestDevice({
        filters:[{services:[NUS_SERVICE]}],
        optionalServices: [NUS_SERVICE]
    });
    gDevice.addEventListener('gattserverdisconnected', () => console.log('BLE disconnected'));

    gServer = await gDevice.gatt!.connect();
    const svc = await gServer.getPrimaryService(NUS_SERVICE);
    rxChar = await svc.getCharacteristic(NUS_RX);
    txChar = await svc.getCharacteristic(NUS_TX);

    await txChar.startNotifications();
    txChar.addEventListener('characteristicvaluechanged', onBleNotification);
    console.log('BLE connected');
}

async function onBleNotification(e: Event) {
    const dv = (e.target as BluetoothRemoteGATTCharacteristic).value!;
    const bytes = new Uint8Array(dv.buffer.slice(dv.byteOffset, dv.byteOffset + dv.byteLength));
    const text = new TextDecoder().decode(bytes); // device → browser data (e.g., JSON or CSV)

    let tJson: bleData = JSON.parse(text)
    //await updateGuidance(tJson.left, tJson.right)

    console.log(tJson+" "+tJson.left+" "+tJson.right)   

    setValue({msg:currentGuidance, front:0, left:tJson.left, right:tJson.right})

    // write processed directive back to device
    //const out = new TextEncoder().encode(JSON.stringify(currentGuidance)); // e.g. {"fan":"ON"}
    //await rxChar!.writeValue(out);
}

export async function sendToDevice(msg: string) {
    const data = new TextEncoder().encode(msg);
    await rxChar!.writeValue(data);
}
