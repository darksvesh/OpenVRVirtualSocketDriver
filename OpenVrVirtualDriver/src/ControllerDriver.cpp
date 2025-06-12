
#define _CRT_SECURE_NO_WARNINGS
#include <ControllerDriver.h>
#include <ctime>
#include <fstream>
#include <string>
#include <ctime>
#include <iostream>

EVRInitError ControllerDriver::Activate(uint32_t unObjectId)
{
	driverId = unObjectId;
	PropertyContainerHandle_t props = VRProperties()->TrackedDeviceToPropertyContainer(driverId); 

	VRProperties()->SetStringProperty(props, Prop_InputProfilePath_String, "controller_profile.json");
	VRProperties()->SetInt32Property(props, Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_Treadmill); 
	VRProperties()->SetBoolProperty(props, Prop_HasControllerComponent_Bool, true);
	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/y", &joystickYHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); 
	VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/y", &trackpadYHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); 
	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/x", &joystickXHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); 
	VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/x", &trackpadXHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); 
	


	VRDriverInput()->CreateScalarComponent(props, "/input/trigger/value", &triggerValueHandle,
		EVRScalarType::VRScalarType_Absolute, EVRScalarUnits::VRScalarUnits_NormalizedOneSided); // Normalized 0..1 for trigger pull
	VRDriverInput()->CreateBooleanComponent(props, "/input/a/click", &aClickHandle);
	VRDriverInput()->CreateBooleanComponent(props, "/input/b/click", &bClickHandle);
	VRDriverInput()->CreateBooleanComponent(props, "/input/x/click", &xClickHandle);
	VRDriverInput()->CreateBooleanComponent(props, "/input/y/click", &yClickHandle);

	axisMap["/input/joystick/y"] = joystickYHandle;
	axisMap["/input/joystick/x"] = joystickXHandle;
	axisMap["/input/trackpad/y"] = trackpadYHandle;
	axisMap["/input/trackpad/x"] = trackpadXHandle;

	axisMap["/input/trigger/value"] = triggerValueHandle;
	buttonMap["/input/a/click"] = aClickHandle;
	buttonMap["/input/b/click"] = bClickHandle;
	buttonMap["/input/x/click"] = xClickHandle;
	buttonMap["/input/y/click"] = yClickHandle;

	return VRInitError_None;
}


void ControllerDriver::SetPosition(float dx, float dy, float dz)
{
	x = dx;
	y = dy;
	z = dz;
}

void ControllerDriver::SetRotation(float dx, float dy, float dz)
{
	qx = dx;
	qy = dy;
	qz = dz;
}


DriverPose_t ControllerDriver::GetPose()
{
	DriverPose_t pose = { 0 };
	pose.poseIsValid = true;
	pose.result = TrackingResult_Running_OK;
	pose.deviceIsConnected = true;
	pose.willDriftInYaw = false;
	pose.shouldApplyHeadModel = false;

	pose.vecPosition[0] = x; // X
	pose.vecPosition[1] = y; // Y
	pose.vecPosition[2] = z; // Z

	pose.qRotation.w = 1;
	pose.qRotation.x = qx;
	pose.qRotation.y = qy;
	pose.qRotation.z = qz;

	return pose;
}

std::string ControllerDriver::bool_to_string(bool value) {
	return value ? "true" : "false";
}

void ControllerDriver::SetButtonState(std::string buttonid, bool state)
{
	auto it = buttonMap.find(buttonid);
	if (it != buttonMap.end()) {
		buttonStates[it->second] = state;
	}
}

void ControllerDriver::SetAxisState(std::string axisid, float state)
{

auto it = axisMap.find(axisid);
	if (it != axisMap.end()) {
		axisStates[it->second] = state;
	}
}

void ControllerDriver::RunFrame()
{
	for (const auto& pair : axisStates)
	{
		VRDriverInput()->UpdateScalarComponent(pair.first, pair.second, 0);
	}

	for (const auto& pair : buttonStates)
	{
		VRDriverInput()->UpdateBooleanComponent(pair.first, pair.second, 0);
	}
}

void ControllerDriver::Deactivate()
{
	driverId = k_unTrackedDeviceIndexInvalid;
}

void* ControllerDriver::GetComponent(const char* pchComponentNameAndVersion)
{
	if (strcmp(IVRDriverInput_Version, pchComponentNameAndVersion) == 0)
	{
		return this;
	}
	return NULL;
}

void ControllerDriver::EnterStandby() {}

void ControllerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) 
{
	if (unResponseBufferSize >= 1)
	{
		pchResponseBuffer[0] = 0;
	}
}