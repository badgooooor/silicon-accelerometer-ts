#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "accel_data.h"

void extract_bytes(const uint8_t *buf, size_t len, AccelerometerData *out) {
    // Extract int32 LE from bytes 6-9
    int32_t x_raw, y_raw, z_raw;
    memcpy(&x_raw, &buf[6],  4);
    memcpy(&y_raw, &buf[10], 4);
    memcpy(&z_raw, &buf[14], 4);

    // Assign value
    out->x = x_raw / ACCEL_SCALE;
    out->y = y_raw / ACCEL_SCALE;
    out->z = z_raw / ACCEL_SCALE;
}

void create_sample_data(AccelerometerData *s, uint64_t timestamp, bool valid, AccelerometerSampleData *out) {
    out->s = *s;
    out->timestamp_ns = timestamp;
    out->valid = valid;
}
