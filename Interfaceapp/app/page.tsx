"use client";

import { onValueChange } from "@/utils/state";

import useMockSensorData from "@/hooks/mockSensorData";
import VoiceAlert from "@/components/voiceAlert";
import DistanceBars from "@/components/distanceBars";
import { connectBLE } from "@/utils/ble";


export default function Page() {
  let data:{ msg:string, front:number, right:number, left:number } = {msg:"clear", front:0, left:0, right:0};

  onValueChange((value) => {
    data=value
  });

  return (
    <main className="flex flex-col items-center justify-center min-h-screen gap-6">
      <h1 className="text-3xl font-bold">VisionAssist Dashboard</h1>

      <button onClick={connectBLE}>Connect Bluetooth</button>
      
      <DistanceBars left={data.left} front={data.front} right={data.right} />
      <div className="text-lg">
        <p>Left Distance: {data.left.toFixed(2)} m</p>
        <p>Front Distance: {data.front.toFixed(2)} m</p>
        <p>Right Distance: {data.right.toFixed(2)} m</p>
        <p>Alert: {data.msg}</p>
      </div>

      <VoiceAlert alert={data.msg} />
    </main>
  );
}
