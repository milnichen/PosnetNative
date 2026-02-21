#ifndef POSNET_PROTOCOL_H
#define POSNET_PROTOCOL_H

#include <string>
#include <vector>

namespace posnet {

// Кодировка протокола Posnet (CP1250)
std::string BuildFrame(const std::string& command);

// Читает ответ из буфера: ACK -> "OK", NAK -> "ERR_CRC", кадр STX...ETX+CRC -> строка тела.
// Возвращает разобранную строку или пустую при неполном кадре.
std::string ParseResponse(const unsigned char* data, size_t length);

// Отправка кадра по TCP и приём ответа. Таймауты в мс.
std::string SendTcpCommand(const std::string& ip, int port, const std::string& command,
    int connectMs = 3000, int sendMs = 3000, int recvMs = 5000);

} // namespace posnet

#endif
