# OpenVRVirtualSocketDriver
I created this driver for my custom devices to work with SteamVR.  
It is simple enough to connect to it using an **ESP32** to control and send input data.

# Release download page
[Release page](https://github.com/darksvesh/OpenVRVirtualSocketDriver/releases/tag/v1)

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

# Esp32 arduino client example
For arduino, based on wificlient lib:
```cpp
if(!client.connect(IPAddress("192.168.1.4"), 9876)){ 
	client.connect("192.168.1.4", 9876)) 
    delay(1000);
    return;
}
client.print("/input/trigger/value=0.75;");
```
# .net client C# example
```c#
TcpClient client = new TcpClient();
client.Connect(serverIp, serverPort);
NetworkStream stream = client.GetStream();
string command = "/input/trigger/value=0.75;";
stream.Write(command, 0, command.Length);
```
# Driver installation
Copy driver files into SteamVR\drivers folder:
```
Steam/steamapps/common/SteamVR/drivers/`<your_driver_name>`/
├── bin/
│ ├── linux64/
│ │ └── driver_`<your_driver_name>`.so (linux)
│ └── win64/
│   └── driver_`<your_driver_name>`.dll
└── resources/
  ├── controller_profile.json
  ├── driver.vrdrivermanifest
  ├── game_controller.svg  
  └── default.vrsettings
```

# Based on
finallyfunctional/openvr-driver-example

# License
```
MIT License

Copyright (c) 2025 darksvesh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
