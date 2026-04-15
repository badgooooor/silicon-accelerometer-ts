#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>

#include "accel_driver.h"

// Integration test with hardware, only just print it's found.
int main(void) {
    IOHIDDeviceRef hid;
    io_service_t service = find_accelerometer_device();
    bool wake_success = false;

    if (service != IO_OBJECT_NULL) {
        printf("Accelerometer found: service handle %d\n", service);
        hid = accel_open_device(service);
        printf("Opened accelerometer: ID %p\n", (void *)hid);
        
        wake_success = accel_wake_device();
        printf("Device wake : %d\n", wake_success);
    } else {
        printf("Accelerometer not found.\n");
    }
        
    return 0;
}
