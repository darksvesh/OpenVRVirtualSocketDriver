#include <DeviceProvider.h>

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext)
{
    EVRInitError initError = InitServerDriverContext(pDriverContext);
    if (initError != EVRInitError::VRInitError_None)
    {
        return initError;
    }
    VRDriverLog()->Log("Initializing controller"); 

    controllerDriver = new ControllerDriver();
    VRServerDriverHost()->TrackedDeviceAdded("Virtual threadmill", TrackedDeviceClass_Controller, controllerDriver);
    remoteServer = new IPCServer(controllerDriver);
    remoteServer->Start();
    return vr::VRInitError_None;
}

void DeviceProvider::Cleanup()
{
    remoteServer->Stop();
    delete remoteServer;
    delete controllerDriver;
    controllerDriver = NULL;
}

const char* const* DeviceProvider::GetInterfaceVersions()
{
    return k_InterfaceVersions;
}

void DeviceProvider::RunFrame()
{
    controllerDriver->RunFrame();
}

bool DeviceProvider::ShouldBlockStandbyMode()
{
    return false;
}

void DeviceProvider::EnterStandby() {}

void DeviceProvider::LeaveStandby() {}
