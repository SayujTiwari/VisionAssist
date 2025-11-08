import { useState, useEffect } from "react";

export interface SensorData {
  front: number;
  left: number;
  alert: string;
}

export default function useMockSensorData(): SensorData {
  const [data, setData] = useState<SensorData>({
    front: 1.0,
    left: 1.0,
    alert: "clear",
  });

  useEffect(() => {
    const interval = setInterval(() => {
      const front = Math.random() * 2;
      const left = Math.random() * 2;
      const alert =
        front < 0.5 ? "stop" : left < 0.5 ? "turn right" : "clear";
      setData({ front, left, alert });
    }, 1500);

    return () => clearInterval(interval);
  }, []);

  return data;
}
