type Listener = (value: procData) => void;

export interface procData {
    msg: string;
    front: number;
    left: number;
    right: number;
}

let value = {msg:"", front:0, left:0, right:0};
const listeners = new Set<Listener>();

export function setValue(v: procData) {
    value = v;
    for (const fn of listeners) fn(v);
}

export function onValueChange(fn: Listener) {
    listeners.add(fn);
    return () => listeners.delete(fn);  // optional unsubscribe
}

export function getValue() {
    return value;
}
