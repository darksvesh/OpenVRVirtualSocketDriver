
#define _CRT_SECURE_NO_WARNINGS
#include <ControllerDriver.h>
#include <ctime>
#include <fstream>
#include <string>
#include <ctime>
#include <iostream>

EVRInitError ControllerDriver::Activate(uint32_t unObjectId)
{
	driverId = unObjectId; //unique ID for your driver

	PropertyContainerHandle_t props = VRProperties()->TrackedDeviceToPropertyContainer(driverId); //this gets a container object where you store all the information about your driver

	VRProperties()->SetStringProperty(props, Prop_InputProfilePath_String, "controller_profile.json"); //tell OpenVR where to get your driver's Input Profile
	VRProperties()->SetInt32Property(props, Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_Treadmill); //tells OpenVR what kind of device this is
	VRProperties()->SetBoolProperty(props, Prop_HasControllerComponent_Bool, true);
	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/y", &joystickYHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //sets up handler you'll use to send joystick commands to OpenVR with, in the Y direction (forward/backward)
	VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/y", &trackpadYHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //sets up handler you'll use to send trackpad commands to OpenVR with, in the Y direction
	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/x", &joystickXHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //Why VRScalarType_Absolute? Take a look at the comments on EVRScalarType.
	VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/x", &trackpadXHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //Why VRScalarUnits_NormalizedTwoSided? Take a look at the comments on EVRScalarUnits.
	


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


std::string ControllerDriver::current_time() {
	std::time_t t = std::time(nullptr);
	char buf[100];
	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
	return std::string(buf);
}

// Функция для логирования
void ControllerDriver::log_to_file(const std::string& message, const std::string& logfile) {
	//std::ofstream log_file(logfile, std::ios::app); // Открытие файла для добавления информации
	//if (log_file.is_open()) {
	//	log_file << ControllerDriver::current_time() << " - " << message << std::endl;
	//}
	//else {
	//	std::cerr << "Ошибка при открытии файла для логирования!" << std::endl;
	//}
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
	//ControllerDriver::log_to_file(buttonid + " " + ControllerDriver::bool_to_string(state), "d:\\1\\liblog.txt");
	auto it = buttonMap.find(buttonid);
	if (it != buttonMap.end()) {
		//ControllerDriver::log_to_file("Found", "d:\\1\\liblog.txt");
		buttonStates[it->second] = state;
	}
}

void ControllerDriver::SetAxisState(std::string axisid, float state)
{

	//ControllerDriver::log_to_file(axisid +" "+ std::to_string(state), "d:\\1\\liblog.txt");
	auto it = axisMap.find(axisid);
	if (it != axisMap.end()) {
		//ControllerDriver::log_to_file("Found", "d:\\1\\liblog.txt");
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