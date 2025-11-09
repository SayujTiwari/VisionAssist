"use client";
import React from "react";

interface DistanceBarsProps {
  left: number;
  front: number;
  right:number;
}

export default function DistanceBars({ left, front, right }: DistanceBarsProps) {
  // Clamp distances to 0–2 m
  const clamp = (val: number) => Math.min(Math.max(val, 0), 200);
  const frontHeight = (1 - clamp(front) / 200) * 100; // invert so nearer → taller
  const leftHeight = left>0? (1 - clamp(left) / 200) * 100 : 0;
  const rightHeight =  right>0?(1 - clamp(right) / 200) * 100:0;

  const getColor = (dist: number) => {
    if (dist < 0.5) return "bg-red-500";
    if (dist < 1.0) return "bg-yellow-400";
    return "bg-green-400";
  };

  return (
    <div className="flex items-end justify-center gap-16 h-64 w-96 bg-neutral-700/60 rounded-xl p-6 shadow-inner">

      {/* Left bar */}
      <div className="flex flex-col items-center justify-end h-full">
        <div
          className={`w-14 transition-all duration-500 rounded-t-md ${getColor(left)}`}
          style={{ height: `${leftHeight}%` }}
        ></div>
        <span className="mt-2 text-sm text-gray-200">Left</span>
      </div>
      {/* Right bar */}
      <div className="flex flex-col items-center justify-end h-full">
        <div
          className={`w-14 transition-all duration-500 rounded-t-md ${getColor(right)}`}
          style={{ height: `${rightHeight}%` }}
        ></div>
        <span className="mt-2 text-sm text-gray-200">right</span>
      </div>
    </div>
  );
}
