import { useState, useEffect } from "react";

export interface SensorData {
  right:number,
  front: number;
  left: number;
  msg: string;
}

export default function useMockSensorData(): SensorData {
  const [data, setData] = useState<SensorData>({
    right: 1.0,
    front: 1.0,
    left: 1.0,
    msg: "clear",
  });

  useEffect(() => {
    const interval = setInterval(() => {
      const front = Math.random() * 2;
      const left = Math.random() * 2;
      const right = Math.random() * 2;
      const msg =
        front < 0.5 ? "stop" : left < 0.5 ? "turn right" : "clear";
      setData({ msg, right, front, left});
    }, 1500);

    return () => clearInterval(interval);
  }, []);

  return data;
}
