import { dlopen, FFIType, ptr } from "bun:ffi";
import { resolve } from "path";

const libs = dlopen(resolve(import.meta.dir, "accel.dylib"), {
	accel_start: {
		args: [],
		returns: FFIType.i32,
	},
	accel_stop: {
		args: [],
		returns: FFIType.void,
	},
	// void accel_read(AccelerometerSampleData *out)
	// AccelerometerSampleData = { x: f64, y: f64, z: f64, ts: u64, valid: bool }
	// Layout: 8 + 8 + 8 + 8 + 1 = 33 bytes (+ padding = 40 bytes)
	accel_read: {
		args: [FFIType.ptr],
		returns: FFIType.void,
	},
});

export class Accelerometer {
	private _started = false;
	private readonly _buffer = new ArrayBuffer(40);

	constructor() {
		process.on("exit", () => {
			if (this._started) libs.symbols.accel_stop();
		});
	}

	start(): void {
		if (this._started) {
			throw new Error("silicon_accelerometer is already started");
		}
		const status = libs.symbols.accel_start();
		if (status !== 0) {
			throw new Error(`Failed to start acceleromter (code: ${status})`);
		}
		this._started = true;
	}

	stop(): void {
		if (!this._started) return;
		libs.symbols.accel_stop();
		this._started = false;
	}

	read(): { x: number; y: number; z: number; timestamp: bigint } | null {
		if (!this._started) {
			throw new Error("silicon_accelerometer has not started. Call start() first");
		}
		libs.symbols.accel_read(ptr(this._buffer));

		const view = new DataView(this._buffer);
		const valid = view.getUint8(32);
		if (!valid) return null;

		return {
			x: view.getFloat64(0, true),
			y: view.getFloat64(8, true),
			z: view.getFloat64(16, true),
			timestamp: view.getBigUint64(24, true),
		};
	}
}
