#pragma once
#include <ControllerDriver.h>
#include <IPCServer.h>
#include <openvr_driver.h>

#ifndef WSOK
#define WSOK
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#endif // !WSOK

using namespace vr;


class DeviceProvider : public IServerTrackedDeviceProvider
{
public:

	EVRInitError Init(IVRDriverContext* pDriverContext);

	void Cleanup();

	const char* const* GetInterfaceVersions();


	void RunFrame();

	bool ShouldBlockStandbyMode();

	void EnterStandby();

	void LeaveStandby();

private:
	ControllerDriver* controllerDriver;
	IPCServer* remoteServer;
};