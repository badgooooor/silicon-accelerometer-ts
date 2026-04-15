#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDManager.h>
#include <CoreFoundation/CoreFoundation.h>

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#include "accel_data.h"

static pthread_mutex_t  s_lock    = PTHREAD_MUTEX_INITIALIZER;
static AccelerometerData  s_latest  = {0};
static AccelerometerSampleData s_latest_expose = {0};
static CFRunLoopRef     s_loop    = NULL;
static pthread_t        s_thread;
static bool             s_running = false;
static uint8_t s_report_buf[4096];

CFStringRef cfstr(const char *s) {
    return CFStringCreateWithCString(kCFAllocatorDefault, s, kCFStringEncodingUTF8);
}

CFNumberRef cfnum32(int32_t v) {
    return CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &v);
}

int64_t prop_int(io_service_t svc, const char *key) {
    CFStringRef k = cfstr(key);
    CFTypeRef ref = IORegistryEntryCreateCFProperty(svc, k, kCFAllocatorDefault, 0);
    CFRelease(k);
    if (!ref) return 0;
    int64_t v = 0;
    CFNumberGetValue((CFNumberRef)ref, kCFNumberSInt64Type, &v);
    CFRelease(ref);
    return v;
}

io_service_t find_accelerometer_device() {
    CFMutableDictionaryRef matching = IOServiceMatching("AppleSPUHIDDevice");
    if (!matching) {
        fprintf(stderr, "IOServiceMatching returned NULL\n");
        return 1;
    }

    io_iterator_t iter;
    kern_return_t kr = IOServiceGetMatchingServices(kIOMainPortDefault, matching, &iter);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "IOServiceGetMatchingServices failed: 0x%x\n", kr);
        return 1;
    }

    bool found = false;
    io_service_t service;
    while ((service = IOIteratorNext(iter)) != IO_OBJECT_NULL) {
        int64_t page = prop_int(service, "PrimaryUsagePage");
        int64_t usage = prop_int(service, "PrimaryUsage");
        
        if (page != PAGE_VENDOR || usage != USAGE_ACCEL) {
            IOObjectRelease(service);
            continue;
        } else {
            IOObjectRelease(iter);
            return service;
        }
    }

    IOObjectRelease(iter);
    return IO_OBJECT_NULL;
}

IOHIDDeviceRef accel_open_device(io_service_t service) {
    IOHIDDeviceRef hid = IOHIDDeviceCreate(kCFAllocatorDefault, service);
    if (!hid) return NULL;

    if (IOHIDDeviceOpen(hid, 0) != kIOReturnSuccess) {
        CFRelease(hid);
        return NULL;
    }

    return hid;
}

bool accel_wake_device() {
    bool found = false;
    io_iterator_t it;
    IOServiceGetMatchingServices(kIOMainPortDefault, IOServiceMatching("AppleSPUHIDDriver"), &it);

    io_service_t service;
    while ((service = IOIteratorNext(it))) {
        found = true;
        const char *keys[] = {
            "SensorPropertyReportingState",         
            "SensorPropertyPowerState",
            "ReportInterval"
        };
        const int32_t values[] = {1, 1, 1000};

        for (int i = 0; i < 3; i++) {
            CFStringRef k = cfstr(keys[i]);
            CFNumberRef v = cfnum32(values[i]);
            IORegistryEntrySetCFProperty(service, k, v);
            CFRelease(k);
            CFRelease(v); 
        }
        IOObjectRelease(service);
    }
    IOObjectRelease(it);

    return found;
}

static void report_callback(
    void *ctx,
    IOReturn result,
    void *sender,
    IOHIDReportType type,
    uint32_t reportID,
    uint8_t *report,
    CFIndex length,
    uint64_t timestamp
) {
    if (length < IMU_DATA_OFF + 12) {
        return;
    }

    pthread_mutex_lock(&s_lock);
    extract_bytes(report, length, &s_latest);
    create_sample_data(&s_latest, timestamp, true, &s_latest_expose);
    pthread_mutex_unlock(&s_lock);
}

static void *accel_run_loop_thread(void *arg) {
    IOHIDDeviceRef hid = (IOHIDDeviceRef)arg;
    IOHIDDeviceRegisterInputReportWithTimeStampCallback(
        hid, s_report_buf, sizeof(s_report_buf), report_callback, NULL
    );
    s_loop = CFRunLoopGetCurrent();
    IOHIDDeviceScheduleWithRunLoop(hid, s_loop, kCFRunLoopDefaultMode);

    while (s_running) {
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.5, false);
    }
    return NULL;
}

int accel_start(void) {
    IOHIDDeviceRef hid;
    bool wake_success = false;
    io_service_t service = find_accelerometer_device();

    if (service != IO_OBJECT_NULL) {
        // Wake the device first before register callbacks 
        // to prevent getting stale data or missing few samples.
        wake_success = accel_wake_device();
        hid = accel_open_device(service);
        if (hid == NULL || !wake_success) {
            return -1;
        }
        
        s_running = true;
        return pthread_create(&s_thread, NULL, accel_run_loop_thread, hid);
    } else {
        return -1;
    }
}

void accel_stop(void) {
    s_running = false;
    if (s_loop) CFRunLoopStop(s_loop);
    pthread_join(s_thread, NULL);
}

AccelerometerSampleData accel_read() {
    AccelerometerSampleData s;
    pthread_mutex_lock(&s_lock);
    s = s_latest_expose;
    pthread_mutex_unlock(&s_lock);
    return s;
}
