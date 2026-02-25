#include "posnet_protocol.h"
#include "posnet_crc.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <netdb.h>
  #define SOCKET int
  #define INVALID_SOCKET (-1)
  #define SOCKET_ERROR (-1)
  #define closesocket close
#endif

namespace posnet {

static const unsigned char STX = 0x02;
static const unsigned char ETX = 0x03;
static const unsigned char ACK = 0x06;
static const unsigned char NAK = 0x15;

// Формат кадра: [STX]command[TAB]#CRC16[ETX]
// command — например "v" или "scomm" (передаётся параметром).
// CRC16 считается от (command + TAB), результат — 4 символа HEX после решётки.
std::string BuildFrame(const std::string& command) {
    std::string body = command;
    if (body.empty() || body.back() != '\t')
        body.push_back('\t');

    unsigned short crc = PosnetCrc16_Compute((const unsigned char*)body.data(), body.size());
    char crcBuf[5];
#if defined(_WIN32) || defined(_WIN64)
    sprintf_s(crcBuf, sizeof(crcBuf), "%04X", crc);
#else
    snprintf(crcBuf, sizeof(crcBuf), "%04X", crc);
#endif

    std::string frame;
    frame.reserve(1 + body.size() + 1 + 4 + 1);
    frame.push_back((char)STX);
    frame.append(body);
    frame.push_back('#');
    frame.append(crcBuf, 4);
    frame.push_back((char)ETX);
    
    std::cout << "DEBUG SEND (No Hash): ";
    for (unsigned char c : frame) printf("%02X ", c);
    std::cout << std::endl;


    return frame;
}

std::string ParseResponse(const unsigned char* data, size_t length) {
    if (!data || length == 0)
        return "";

    unsigned char first = data[0];
    if (first == ACK)
        return "OK";
    if (first == NAK)
        return "ERR_CRC";

    if (first == STX) {
        const unsigned char* p = data + 1;
        size_t avail = length - 1;
        size_t i = 0;
        while (i < avail && p[i] != ETX)
            i++;
        if (i >= avail || i + 1 + 4 > avail)
            return ""; /* неполный кадр */

        size_t bodyLen = i; 
        unsigned short crcReceived;
#if defined(_WIN32) || defined(_WIN64)
        if (sscanf_s((const char*)(p + bodyLen), "%4hX", &crcReceived) != 1)
#else
        if (sscanf((const char*)(p + bodyLen), "%4hX", &crcReceived) != 1)
#endif
            return "ERR_CRC_FORMAT";

        unsigned short crcCalc = PosnetCrc16_Compute(p, bodyLen);
        if (crcCalc != crcReceived)
            return "ERR_CRC_RESPONSE";

        return std::string((const char*)p, i);
    }

    return std::string((const char*)data, length);
}

std::string SendTcpCommand(const std::string& ip, int port, const std::string& command,
    int connectMs, int sendMs, int recvMs) {

#if defined(_WIN32) || defined(_WIN64)
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return "ERR_SYSTEM: WSAStartup";
#endif
    //std::string commandToDevice = "getrealid";
    std::string frame = BuildFrame(command);
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
#if defined(_WIN32) || defined(_WIN64)
        WSACleanup();
#endif
        return "ERR_SYSTEM: socket";
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
        closesocket(s);
#if defined(_WIN32) || defined(_WIN64)
        WSACleanup();
#endif
        return "ERR_PARAM_IP";
    }

#if defined(_WIN32) || defined(_WIN64)
    DWORD tv_conn = (DWORD)connectMs;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_conn, sizeof(tv_conn));
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        closesocket(s);
        WSACleanup();
        return "ERR_TIMEOUT_CONNECT";
    }
    DWORD tv_send = (DWORD)sendMs;
    DWORD tv_recv = (DWORD)recvMs;
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv_send, sizeof(tv_send));
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_recv, sizeof(tv_recv));
#else
    struct timeval tv;
    tv.tv_sec = connectMs / 1000;
    tv.tv_usec = (connectMs % 1000) * 1000;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        closesocket(s);
        return "ERR_TIMEOUT_CONNECT";
    }
    tv.tv_sec = sendMs / 1000;
    tv.tv_usec = (sendMs % 1000) * 1000;
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    tv.tv_sec = recvMs / 1000;
    tv.tv_usec = (recvMs % 1000) * 1000;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif

    size_t sent = 0;
    while (sent < frame.size()) {
        int n = (int)send(s, frame.data() + sent, (int)(frame.size() - sent), 0);
        if (n <= 0) {
            closesocket(s);
#if defined(_WIN32) || defined(_WIN64)
            WSACleanup();
#endif
            return "ERR_SYSTEM: send";
        }
        sent += (size_t)n;
    }

    unsigned char buffer[512];
    std::vector<unsigned char> accumulated;
    std::string result = "ERR_NO_RESPONSE";

    while (1) {
        int n = recv(s, (char*)buffer, sizeof(buffer), 0);
        if (n <= 0)
            break;
        accumulated.insert(accumulated.end(), buffer, buffer + n);
        if (accumulated.empty())
            continue;

        std::string parsed = ParseResponse(accumulated.data(), accumulated.size());
        if (!parsed.empty()) {
            result = parsed;
            break;
        }
    }

    if (result == "ERR_NO_RESPONSE" && !accumulated.empty()) {
        result = std::string(accumulated.begin(), accumulated.end());
        while (!result.empty() && (result.back() == '\r' || result.back() == '\n' || result.back() == ' '))
            result.pop_back();
    }

    closesocket(s);
#if defined(_WIN32) || defined(_WIN64)
    WSACleanup();
#endif
    return result;
}

} // namespace posnet
