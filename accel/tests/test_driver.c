#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>

#include "accel_driver.h"

// Integration test with hardware, only just print it's found.
int main(void) {
    int start_status = accel_start();
    sleep(2);

    for (int i = 0; i < 10; i++) {
        AccelerometerSampleData data;
        accel_read(&data);

        if (data.valid) {
            printf("accel: x=%.4f y=%.4f z=%.4f ts=%llu ns\n",
               data.s.x, data.s.y, data.s.z, data.timestamp_ns);
        } else {
            printf("accel: no valid sample\n");
        }
        sleep(1);
    }

    return 0;
}
