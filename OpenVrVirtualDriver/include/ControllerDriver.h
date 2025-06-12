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

/**
This class controls the behavior of the controller. This is where you 
tell OpenVR what your controller has (buttons, joystick, trackpad, etc.).
This is also where you inform OpenVR when the state of your controller 
changes (for example, a button is pressed).

For the methods, take a look at the comment blocks for the ITrackedDeviceServerDriver 
class too. Those comment blocks have some good information.

This example driver will simulate a controller that has a joystick and trackpad on it.
It is hardcoded to just return a value for the joystick and trackpad. It will cause 
the game character to move forward constantly.
**/
class ControllerDriver : public ITrackedDeviceServerDriver
{
public:

	/**
	Initialize your controller here. Give OpenVR information 
	about your controller and set up handles to inform OpenVR when 
	the controller state changes.
	**/
	EVRInitError Activate(uint32_t unObjectId);

	/**
	Un-initialize your controller here.
	**/
	void SetButtonState(std::string buttonid, bool state);

	void SetAxisState(std::string axisid, float state);

	void SetPosition(float dx, float dy, float dz);

	void SetRotation(float qx, float qy, float qz);

	void Deactivate();

	/**
	Tell your hardware to go into stand-by mode (low-power).
	**/
	void EnterStandby();

	/**
	Take a look at the comment block for this method on ITrackedDeviceServerDriver. So as far 
	as I understand, driver classes like this one can implement lots of functionality that 
	can be categorized into components. This class just acts as an input device, so it will 
	return the IVRDriverInput class, but it could return other component classes if it had 
	more functionality, such as maybe overlays or UI functionality.
	**/
	void* GetComponent(const char* pchComponentNameAndVersion);

	/**
	Refer to ITrackedDeviceServerDriver. I think it sums up what this does well.
	**/
	void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);

	/**
	Returns the Pose for your device. Pose is an object that contains the position, rotation, velocity, 
	and angular velocity of your device.
	**/
	DriverPose_t GetPose();

	/**
	You can retrieve the state of your device here and update OpenVR if anything has changed. This 
	method should be called every frame.
	**/
	void RunFrame();

private:
	std::string current_time();
	void log_to_file(const std::string& message, const std::string& logfile);
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