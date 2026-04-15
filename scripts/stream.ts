import { Accelerometer } from "../accel";

const accel = new Accelerometer();
accel.start();

process.on("SIGINT", () => {
    accel.stop();
    process.exit(0);
});

console.log("Streaming accelerometer data — press Ctrl+C to stop\n");

await Bun.sleep(2000);

while (true) {
    const sample = accel.read();

    if (sample) {
        const { x, y, z, timestamp } = sample;
        const fmt = (n: number) => n.toFixed(4).padStart(7);
        console.log(`accel: x=${fmt(x)}  y=${fmt(y)}  z=${fmt(z)}  ts=${timestamp} ns`);
    } else {
        console.log("accel: no valid sample");
    }

    await Bun.sleep(1000);
}
