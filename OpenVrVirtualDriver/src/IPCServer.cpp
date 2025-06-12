#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCKAPI_
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "IPCServer.h">





#include <ControllerDriver.h>


std::mutex logMutex;
std::mutex clientMutex;

IPCServer::IPCServer(ControllerDriver* device) : m_Device(device), m_Running(false) {}







void IPCServer::Start() {
    m_Running = true;
    m_Thread = std::thread(&IPCServer::ServerLoop, this);
}

void IPCServer::Stop() {
    m_Running = false;
    std::lock_guard<std::mutex> lock(clientMutex);
    {
        for (auto& clientThread : m_ClientThreads) {
            if (clientThread.joinable()) {
                clientThread.join();
            }
        }
        m_ClientThreads.clear(); 
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
        return;
    }

    std::string path = input.substr(0, equalPos);
    std::string valueStr = input.substr(equalPos + 1);

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
        }
    }
}

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
    serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    serverAddr.sin_port = htons(9876);

    bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);
    u_long mode = 1; 
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