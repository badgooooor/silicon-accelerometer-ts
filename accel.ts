import { dlopen, FFIType, ptr, toArrayBuffer } from "bun:ffi";
import { resolve } from "path";

const lib = dlopen(resolve(import.meta.dir, "accel.dylib"), {
     accel_start: {
        args: [],
        returns: FFIType.i32,
     },
     accel_stop: {
        args: [],
        returns: FFIType.void
     },
     // void accel_read(AccelerometerSampleData *out)
     // AccelerometerSampleData = { x: f64, y: f64, z: f64, ts: u64, valid: bool }
     // Layout: 8 + 8 + 8 + 8 + 1 = 33 bytes (+ padding = 40 bytes)
     accel_read: {
        args: [FFIType.ptr],
        returns: FFIType.void
     }
});

// Allocate data buffer based on AccelerometerSampleData
const ACCELEROMETER_SAMPLE_DATA = 40;
const buffer = new ArrayBuffer(ACCELEROMETER_SAMPLE_DATA);

const { symbols } = lib;

let _started = false;

export const start = () => {
     if (_started) {
          throw new Error('silicon_accelerometer is already started');
     }
     const status = symbols.accel_start();
     if (status !== 0) {
          throw new Error(`Failed to start acceleromter (code: ${status})`)
     }
     _started = true;
};

export const stop = () => {
     if (!_started) return;
     symbols.accel_stop();
     _started = false;
}

export const read = (): { x: number; y: number; z: number; timestamp: bigint } | null => {
     if (!_started) {
          throw new Error('silicon_accelerometer has not started. Call start() first');
     }
     symbols.accel_read(ptr(buffer));

     const view = new DataView(buffer);
     const valid = view.getUint8(32);
     if (!valid) return null;

     return {
          x:         view.getFloat64(0,  true),
          y:         view.getFloat64(8,  true),
          z:         view.getFloat64(16, true),
          timestamp: view.getBigUint64(24, true),
     };
}

process.on("exit", () => {
    if (_started) symbols.accel_stop();
});
