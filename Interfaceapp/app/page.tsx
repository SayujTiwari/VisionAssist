"use client";

import { useEffect, useState } from "react";
import { onValueChange, getValue, type procData } from "@/utils/state";
import { connectBLE } from "@/utils/ble";
import VoiceAlert from "@/components/voiceAlert";
import DistanceBars from "@/components/distanceBars";

export default function Page() {
  const [data, setData] = useState<procData>(() => getValue() ?? { msg: "clear", front: 0, left: 0, right: 0 });

  // Subscribe once to your state emitter; unsubscribe on unmount
  useEffect(() => {
    const off = onValueChange((value) => setData(value));
    return () => { if (typeof off === "function") off(); };
  }, []);

  return (
    <main className="flex flex-col items-center justify-center min-h-screen gap-6 p-6">
      <h1 className="text-3xl font-bold">VisionAssist Dashboard</h1>

      <button className="px-4 py-2 rounded-xl bg-black text-white" onClick={connectBLE}>
        Connect Bluetooth
      </button>

      <DistanceBars front={data.front} left={data.trench} right={data.angle} />

      <div className="text-lg">
        <p>Front Distance: {Number.isFinite(data.front) ? data.front.toFixed(2) : "--"} m</p>
        <p>Trench Distance: {Number.isFinite(data.trench) ? data.trench.toFixed(2) : "--"} m</p>
        <p>Angle: {Number.isFinite(data.angle) ? data.angle.toFixed(0) : "--"}°</p>
        <p>Alert: {data.msg}</p>
      </div>


      <VoiceAlert alert={data.msg} />
    </main>
  );
}
