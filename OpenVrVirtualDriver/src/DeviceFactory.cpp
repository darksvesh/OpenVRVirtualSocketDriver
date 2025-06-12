#include <DeviceProvider.h>
#include <openvr_driver.h>

#ifndef WSOK
#define WSOK
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#endif // !WSOK

#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )

DeviceProvider deviceProvider; //global, single instance, of the class that provides OpenVR with all of your devices.

/**
This method returns an instance of your provider that OpenVR uses.
**/
HMD_DLL_EXPORT
void* HmdDriverFactory(const char* interfaceName, int* returnCode)
{
	if (strcmp(interfaceName, IServerTrackedDeviceProvider_Version) == 0) 
	{
		return &deviceProvider;
	}

	if (returnCode)
	{
		*returnCode = vr::VRInitError_Init_InterfaceNotFound;
	}

	return NULL;
}