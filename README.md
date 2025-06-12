# OpenVRVirtualSocketDriver
I created this driver for my custom devices to work with SteamVR.  
It is simple enough to connect to it using an **ESP32** to control and send input data.

# It supports:
| Control element    | Description      | Input path              |
|--------------------|------------------|-------------------------|
| Joystick (Axis X)  | Position X       | `/input/joystick/x`     |
| Joystick (Axis Y)  | Position Y       | `/input/joystick/y`     |
| Trackpad (Axis X)  | Position Y       | `/input/trackpad/x`     |
| Trackpad (Axis Y)  | Position X       | `/input/trackpad/y`     |
| Trigger            | Trigger value    | `/input/trigger/value`  |
| Button A           | Button click A   | `/input/a/click`        |
| Button B           | Button click B   | `/input/b/click`        |
| Button X           | Button click X   | `/input/x/click`        |
| Button Y           | Button click Y   | `/input/y/click`        |

# Usage:

To communicate with the server:
- Connect to **port 9876**.
- Send data using the following format:
`<path>`=`<value>`;
- Where:
  - `<path>` — one of the input paths listed in the table above.
  - `<value>` — the control value.
  - `;` — mandatory command terminator (end of line / command delimiter).
  
| Control Type   | Value Format        | Range            |
|----------------|---------------------|-------------------|
| Buttons        | boolean             | `true` or `false` |
| Trigger        | floating point      | `0.0` to `1.0`    |
| Joystick / Trackpad (axis) | floating point | `-1.0` to `1.0`  |

Like:
/input/trigger/value=0.75;
/input/a/click=true;

# Esp32 arduino example
For arduino, based on wificlient lib:
```cpp
if(!client.connect(IPAddress("192.168.1.4"), 10000)){ 
	client.connect("192.168.1.4", 10000)) 
    delay(1000);
    return;
}
client.print("/input/trigger/value=0.75;");
```
# .net C# example
```c#
TcpClient client = new TcpClient();
client.Connect(serverIp, serverPort);
NetworkStream stream = client.GetStream();
string command = "/input/trigger/value=0.75;";
stream.Write(command, 0, command.Length);
```
# Driver installation
Copy driver lib into SteamVR\drivers 
driver folder hierarchy:
```
Steam/steamapps/common/SteamVR/drivers/`<your_driver_name>`/
├── bin/
│ ├── linux64/
│ │ └── driver_`<your_driver_name>`.so
│ └── win64/
│   └── driver_`<your_driver_name>`.dll
└── resources/
  ├── controller_profile.json
  ├── driver.vrdrivermanifest
  ├── game_controller.svg  
  └── default.vrsettings
```
