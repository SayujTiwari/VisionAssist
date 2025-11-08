"use client";
import useMockSensorData from "@/hooks/mockSensorData";
import VoiceAlert from "@/components/voiceAlert";

export default function Page() {
  const { front, left, alert } = useMockSensorData();

  return (
    <main className="flex flex-col items-center justify-center min-h-screen gap-6">
      <h1 className="text-3xl font-bold">VisionAssist Dashboard</h1>

      <div className="text-lg">
        <p>Front Distance: {front.toFixed(2)} m</p>
        <p>Left Distance: {left.toFixed(2)} m</p>
        <p>Alert: {alert}</p>
      </div>

      <VoiceAlert alert={alert} />
    </main>
  );
}
