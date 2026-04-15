# silicon-accelerometer

Read accelerometer data from Apple Silicon Macs via Bun FFI.

A TypeScript wrapper around a C implementation based from [apple-silicon-accelerometer](https://github.com/olvvier/apple-silicon-accelerometer), exposing `x`, `y`, `z`, and `timestamp` readings through a simple class interface.

## Requirements

- macOS, Apple Silicon (arm64)
- [Bun](https://bun.sh)

## Install

```sh
bun add silicon-accelerometer
```

## Usage

```ts
import { Accelerometer } from "silicon-accelerometer";

const accel = new Accelerometer();
accel.start();

const reading = accel.read();
// { x: 0.01, y: -0.03, z: 9.81, timestamp: 1234567890n }

accel.stop();
```

`read()` returns `null` if no valid sample is available. `stop()` is called automatically on process exit.

## API

| Method | Description |
|--------|-------------|
| `start()` | Start the accelerometer. Throws if already started. |
| `stop()` | Stop and release the native library. Safe to call multiple times. |
| `read()` | Return `{ x, y, z, timestamp }` or `null` if no valid sample. |

## Build

To recompile the native dylib:

```sh
bun run build:dylib
```

## License

MIT
