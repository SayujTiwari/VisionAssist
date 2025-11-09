// utils/state.ts
export interface procData {
  msg: string;
  front: number;
  left: number;
  right: number;
}

type Listener = (data: procData) => void;
let current: procData = { msg: "clear", front: 0, left: 0, right: 0 };
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

export function setValue(data: procData) {
  current = data;
  listeners.forEach((f) => f(data));
}
