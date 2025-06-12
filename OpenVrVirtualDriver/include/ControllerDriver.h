#pragma once
#include <openvr_driver.h>
#include <unordered_map>

#ifndef WSOK
#define WSOK
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#endif // !WSOK


using namespace vr;


class ControllerDriver : public ITrackedDeviceServerDriver
{
public:


	EVRInitError Activate(uint32_t unObjectId);

	void SetButtonState(std::string buttonid, bool state);

	void SetAxisState(std::string axisid, float state);

	void SetPosition(float dx, float dy, float dz);

	void SetRotation(float qx, float qy, float qz);

	void Deactivate();

	void EnterStandby();

	void* GetComponent(const char* pchComponentNameAndVersion);


	void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);

	DriverPose_t GetPose();

	void RunFrame();

private:
	std::string bool_to_string(bool value);

	uint32_t driverId;

	std::unordered_map<std::string, VRInputComponentHandle_t> axisMap;
	std::unordered_map<std::string, VRInputComponentHandle_t> buttonMap;
	std::unordered_map<VRInputComponentHandle_t, float> axisStates;
	std::unordered_map<VRInputComponentHandle_t, bool> buttonStates;

	float x, y, z;
	float qx, qy, qz;

	VRInputComponentHandle_t joystickYHandle;
	VRInputComponentHandle_t trackpadYHandle;
	VRInputComponentHandle_t joystickXHandle;
	VRInputComponentHandle_t trackpadXHandle;
	VRInputComponentHandle_t triggerValueHandle;
	VRInputComponentHandle_t aClickHandle;
	VRInputComponentHandle_t bClickHandle;
	VRInputComponentHandle_t xClickHandle;
	VRInputComponentHandle_t yClickHandle;

};