#include <stdio.h>

int main() {
    unsigned char data = 0x76; // 'v'
    unsigned int crc = 0x0000; // Используем int для надежности расчетов

    printf("Start CRC: #%04X\n", crc & 0xFFFF);

    crc ^= (unsigned int)data << 8;
    printf("After XOR with data: #%04X\n", crc & 0xFFFF);

    for (int j = 0; j < 8; j++) {
        if (crc & 0x8000) {
            crc = (crc << 1) ^ 0x1021;
        } else {
            crc <<= 1;
        }
        // ОГРОМНАЯ ВАЖНОСТЬ: Отрезаем всё, что выше 16 бит, на каждом шаге
        crc &= 0xFFFF; 
        
        printf("Step %d: #%04X\n", j + 1, crc);
    }

    printf("\nFinal Result: #%04X\n", crc);
    return 0;
}