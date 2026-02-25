#include "posnet_crc.h"

/* CRC-16 (CCITT), полином 0x1021, как в протоколе Posnet */
static unsigned short s_crc_table[256];
static int s_crc_table_initialized = 0;

static void PosnetCrc16_InitTable(void) {
    const unsigned short poly = 0x1021;
    for (unsigned int i = 0; i < 256; i++) {
        unsigned short value = 0;
        unsigned short temp = (unsigned short)(i << 8);
        for (int j = 0; j < 8; j++) {
            if (((value ^ temp) & 0x8000) != 0)
                value = (unsigned short)((value << 1) ^ poly);
            else
                value = (unsigned short)(value << 1);
            temp = (unsigned short)(temp << 1);
        }
        s_crc_table[i] = value;
    }
    s_crc_table_initialized = 1;
}

unsigned short PosnetCrc16_Compute(const unsigned char* data, size_t length) {
    if (!data || length == 0)
        return 0;

    /*if (!s_crc_table_initialized)
        PosnetCrc16_InitTable();

    unsigned short crc = 0xFFFF;
    for (size_t i = 0; i < length; i++) {
        unsigned char b = data[i];
        crc = (unsigned short)((crc << 8) ^ s_crc_table[((crc >> 8) ^ b) & 0xFF]);
    }*/
   unsigned short crc = 0xFFFF; 

    for (size_t i = 0; i < length; i++) {
        // XOR старшего байта CRC с входящим байтом данных
        crc ^= (unsigned short)data[i] << 8;
        
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}
