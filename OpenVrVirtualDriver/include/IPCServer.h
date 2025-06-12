
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCKAPI_
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef WSOK
#define WSOK
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#endif // !WSOK


#include "ControllerDriver.h"

#include <thread>
#include <atomic>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <mutex>

class IPCServer {
public:
    IPCServer(ControllerDriver* device);
    void Start();
    void Stop();

private:
    void ProcessInput(const std::string& input);
    void ServerLoop();
    void HandleClient(SOCKET clientSocket);
    std::thread m_Thread;
    std::atomic<bool> m_Running;
    ControllerDriver* m_Device;
    std::vector<std::thread> m_ClientThreads;

};
