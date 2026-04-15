#ifndef ACCEL_DRIVER_H
#define ACCEL_DRIVER_H

#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDManager.h>
#include <CoreFoundation/CoreFoundation.h>

#include "accel_data.h"

// Utilities.
CFStringRef     cfstr(const char *s);
CFNumberRef     cfnum32(int32_t v);
int64_t         prop_int(io_service_t svc, const char *key);

// Device discovery and activation.
io_service_t    find_accelerometer_device();
IOHIDDeviceRef  accel_open_device(io_service_t service);
bool            accel_wake_device(void);

// Start/stop loop and read sensor values.
int                                 accel_start(void);
void                                accel_stop(void);
void                                accel_read(AccelerometerSampleData *out);

#endif
