#ifndef ACCEL_H
#define ACCEL_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#define IMU_DATA_OFF 6
#define ACCEL_SCALE  65536.0

// AppleSPUHIDDevice Accelerometer
#define PAGE_VENDOR   0xFF00
#define USAGE_ACCEL   3

typedef struct {
    double x, y, z;
} AccelerometerData;

typedef struct {
    AccelerometerData s;
    uint64_t timestamp_ns;
    bool valid;
} AccelerometerSampleData;

void extract_bytes(const uint8_t *buf, size_t len, AccelerometerData *out);
void create_sample_data(AccelerometerData *s, uint64_t timestamp, bool valid, AccelerometerSampleData *out);

#endif
