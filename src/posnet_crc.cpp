#include "posnet_crc.h"

unsigned short PosnetCrc16_Compute(const unsigned char* data, size_t length) {
    if (!data || length == 0) return 0;
    unsigned int crc = 0x0000; // Для Posnet Standard

    for (size_t i = 0; i < length; i++) {
        crc ^= (unsigned int)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000)
                crc = ((crc << 1) ^ 0x1021) & 0xFFFF;
            else
                crc = (crc << 1) & 0xFFFF;
        }
    }
    return (unsigned short)crc;
}