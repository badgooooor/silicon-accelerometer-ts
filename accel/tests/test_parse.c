#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "accel_data.h"

void test_parse_1g_x() {
    uint8_t buf[22] = {0};
    // 1.0g = 65536 = 0x00010000 little-endian
    buf[6] = 0x00;
    buf[7] = 0x00;
    buf[8] = 0x01;
    buf[9] = 0x00;

    AcceleratorData s;
    extract_bytes(buf, 22, &s);

    assert(fabs(s.x - 1.0) < 1e-9);
    printf("PASS: test_parse_1g_x\n");
}

void test_parse_1g_y() {
    uint8_t buf[22] = {0};
    // 1.0g = 65536 = 0x00010000 little-endian at bytes 10-13
    buf[10] = 0x00;
    buf[11] = 0x00;
    buf[12] = 0x01;
    buf[13] = 0x00;

    AcceleratorData s;
    extract_bytes(buf, 22, &s);

    assert(fabs(s.y - 1.0) < 1e-9);
    printf("PASS: test_parse_1g_y\n");
}

void test_parse_1g_z() {
    uint8_t buf[22] = {0};
    // 1.0g = 65536 = 0x00010000 little-endian at bytes 14-17
    buf[14] = 0x00;
    buf[15] = 0x00;
    buf[16] = 0x01;
    buf[17] = 0x00;

    AcceleratorData s;
    extract_bytes(buf, 22, &s);

    assert(fabs(s.z - 1.0) < 1e-9);
    printf("PASS: test_parse_1g_z\n");
}

void test_parse_negative_g() {
    uint8_t buf[22] = {0};
    // -1.0g = -65536 = 0xFFFF0000 little-endian at bytes 6-9
    buf[6] = 0x00;
    buf[7] = 0x00;
    buf[8] = 0xFF;
    buf[9] = 0xFF;

    AcceleratorData s;
    extract_bytes(buf, 22, &s);

    assert(fabs(s.x - (-1.0)) < 1e-9);
    printf("PASS: test_parse_negative_g\n");
}

int main() {
    test_parse_1g_x();
    test_parse_1g_y();
    test_parse_1g_z();
    test_parse_negative_g();
    return 0;
}
