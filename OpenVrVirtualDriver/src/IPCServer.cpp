
// ============================
// IPCServer.cpp
// ============================

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCKAPI_
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "IPCServer.h">


#ifndef WSOK
//#define WSOK
//#include <windows.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#pragma comment(lib, "Ws2_32.lib")
#endif // !WSOK


#include <ControllerDriver.h>


std::mutex logMutex;
std::mutex clientMutex;

IPCServer::IPCServer(ControllerDriver* device) : m_Device(device), m_Running(false) {}


std::string IPCServer::current_time() {
  //  std::time_t t = std::time(nullptr);
  //  char buf[100];
 //   std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
  //  return std::string(buf);
    return "";
}

// Функция для логирования
void IPCServer::log_to_file(const std::string& message, const std::string& logfile) {
    //std::lock_guard<std::mutex> lock(logMutex);
   // std::ofstream log_file(logfile, std::ios::app); // Открытие файла для добавления информации
   // if (log_file.is_open()) {
   //     log_file << IPCServer::current_time() << " - " << message << std::endl;
  //  }
  //  else {
   //     std::cerr << "Ошибка при открытии файла для логирования!" << std::endl;
  //  }
}



void IPCServer::Start() {
    m_Running = true;
    m_Thread = std::thread(&IPCServer::ServerLoop, this);
}

void IPCServer::Stop() {
    m_Running = false;
    // Ожидаем завершения всех потоков
    std::lock_guard<std::mutex> lock(clientMutex);
    {
        for (auto& clientThread : m_ClientThreads) {
            if (clientThread.joinable()) {
                clientThread.join();
            }
        }
        m_ClientThreads.clear(); // Удалим все завершённые потоки
    }
    if (m_Thread.joinable()) m_Thread.join();
}

void IPCServer::ProcessInput(const std::string& input)
{
    //position
    if (input.find("pos:") != std::string::npos) {
        float x, y, z;
        if (sscanf(input.c_str(), "pos:%f,%f,%f", &x, &y, &z) == 3) {
            m_Device->SetPosition(x, y, z);
        }
        return;
    }
    //rotation
    if (input.find("rot:") != std::string::npos) {
        float x, y, z;
        if (sscanf(input.c_str(), "pos:%f,%f,%f", &x, &y, &z) == 3) {
            m_Device->SetRotation(x, y, z);
        }
        return;
    }

    size_t equalPos = input.find('=');
    if (equalPos == std::string::npos) {
     //   std::cerr << "Invalid input format: " << input << std::endl;
        return;
    }

    std::string path = input.substr(0, equalPos);
    std::string valueStr = input.substr(equalPos + 1);

    // Determine if the value is boolean or float
    if (valueStr == "true" || valueStr == "false") {
        bool state = (valueStr == "true");
        m_Device->SetButtonState(path, state);
    }
    else {
        try {
            float value = std::stof(valueStr);
            m_Device->SetAxisState(path, value);
        }
        catch (const std::invalid_argument&) {
         //   std::cerr << "Invalid numeric value: " << valueStr << std::endl;
        }
    }
}

// Функция обработки каждого клиента
void IPCServer::HandleClient(SOCKET clientSocket) {
    char buffer[512];
    std::string dataBuffer;
    int bytesReceived = 0;
    while (m_Running) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK) {
                continue;
            }
            else {
                break;
            }
        }
        buffer[bytesReceived] = '\0';
        dataBuffer += buffer;
        size_t pos;
        while ((pos = dataBuffer.find(';')) != std::string::npos) {
            std::string line = dataBuffer.substr(0, pos);
            dataBuffer.erase(0, pos + 1);
            this->ProcessInput(line);
        }
    }
    closesocket(clientSocket);
}

void IPCServer::ServerLoop() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(9876);

    bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);
    u_long mode = 1; // 1 = non-blocking
    ioctlsocket(serverSocket, FIONBIO, &mode);
    fd_set readfds;
    timeval timeout;

    while (m_Running) {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 50000; // 50ms

        int activity = select(0, &readfds, NULL, NULL, &timeout);
        if (activity > 0 && FD_ISSET(serverSocket, &readfds)) {
            SOCKET clientSocket = accept(serverSocket, NULL, NULL);

            if (clientSocket == INVALID_SOCKET) continue;


            std::lock_guard<std::mutex> lock(clientMutex);
            m_ClientThreads.push_back(std::thread([this, clientSocket]() {
                HandleClient(clientSocket);
                }));
        }
    }

    closesocket(serverSocket);
    WSACleanup();
}