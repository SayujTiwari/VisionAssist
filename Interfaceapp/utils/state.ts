// utils/state.ts
export interface procData {
  front: number;       // derived from sensorObstacleRaw (m)
  trench: number;      // derived from sensorTrench (m)
  angle: number;
  horizontal: number;
  msg: string;
}

type Listener = (data: procData) => void;

let current: procData = {
  front: 0,
  trench: 0,
  angle: 0,
  horizontal: 0,
  msg: "waiting...",
};

let listeners: Listener[] = [];

export function getValue() {
  return current;
}

export function onValueChange(cb: Listener) {
  listeners.push(cb);
  return () => {
    listeners = listeners.filter((f) => f !== cb);
  };
}

export function emitValue(data: procData) {
  current = data;
  listeners.forEach((f) => f(data));
}
