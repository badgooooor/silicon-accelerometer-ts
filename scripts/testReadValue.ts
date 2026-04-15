import { start, stop, read } from "../accel";

start();
await Bun.sleep(2000);

for (let i = 0; i < 10; i++) {
    const sample = read();

    if (sample) {
        const { x, y, z, timestamp } = sample;
        const fmt = (n: number) => n.toFixed(4).padStart(7);
        console.log(`accel: x=${fmt(x)}  y=${fmt(y)}  z=${fmt(z)}  ts=${timestamp} ns`);
    } else {
        console.log("accel: no valid sample");
    }

    await Bun.sleep(1000);
}

stop();
