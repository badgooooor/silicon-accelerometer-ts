#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDManager.h>
#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>

#include "accel.h"

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
