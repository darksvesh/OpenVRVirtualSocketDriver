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
    VRServerDriverHost()->TrackedDeviceAdded("Creepybox virtual threadmill", TrackedDeviceClass_Controller, controllerDriver); //add all your devices like this.
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