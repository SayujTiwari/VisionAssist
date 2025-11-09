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

      <DistanceBars left={data.left} front={data.front} right={data.right} />

      <div className="text-lg">
        <p>Left Distance: {Number.isFinite(data.left) ? data.left.toFixed(2) : "—"} m</p>
        <p>Front Distance: {Number.isFinite(data.front) ? data.front.toFixed(2) : "—"} m</p>
        <p>Right Distance: {Number.isFinite(data.right) ? data.right.toFixed(2) : "—"} m</p>
        <p>Alert: {data.msg}</p>
      </div>

      <VoiceAlert alert={data.msg} />
    </main>
  );
}
