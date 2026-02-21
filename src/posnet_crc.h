#ifndef POSNET_CRC_H
#define POSNET_CRC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned short PosnetCrc16_Compute(const unsigned char* data, size_t length);

#ifdef __cplusplus
}
#endif

#endif
