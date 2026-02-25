#include <iostream>
#include <string>

// 1. Сначала объявляем, что такая функция существует внутри этого пространства имен
namespace posnet {
    std::string SendTcpCommand(const std::string& ip, int port, const std::string& command, 
                               int connectMs, int sendMs, int recvMs);
}

int main() {
    // Настраиваем параметры под ваш принтер
    std::string ip = "192.168.30.80"; 
    int port = 6666;                 
    std::string cmd = "getrealid";           

    std::cout << "--- Starting Posnet Test ---" << std::endl;
    std::cout << "Target: " << ip << ":" << port << std::endl;
    std::cout << "Command: [" << cmd << "]" << std::endl;

    // 2. Теперь вызываем через имя пространства
    std::string response = posnet::SendTcpCommand(ip, port, cmd, 2000, 2000, 2000);

    std::cout << "----------------------------" << std::endl;
    std::cout << "Printer Response: " << response << std::endl;
    std::cout << "----------------------------" << std::endl;
    
    return 0;
}