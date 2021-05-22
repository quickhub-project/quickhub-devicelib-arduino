# The QuickHub Device Library for Arduino

With the help of this library, IoT devices based on the low-cost Espressif microcontrollers can be easily created. With just a few lines of code, sensors and actuators can be connected to a QuickHub server and then controlled from anywhere. 

## Dependencies

- ArduinoJson (version 5.x) by Benoît Blanchon https://github.com/bblanchon/ArduinoJson/tree/5.x
- ArduinoWebsockets by Mathias Sattler https://github.com/Links2004/arduinoWebSockets.git
- WifiManager https://github.com/tzapu/WiFiManager.git

## Setup with PlatformIO

Just add the following lines to your ```platformio.ini```

```
lib_deps =  https://github.com/bblanchon/ArduinoJson.git#5.x
            https://github.com/tzapu/WiFiManager.git
            549
            
```

## Example

This example implements a readwrite property which allows to switch the internal LED of the Wemos D1 mini on and off.

```c
#include <Arduino.h>
#include <DeviceNode.h>
#include <ArduinoWebsocketsWrapper.h>
#include <WifiLoader.h>

// Instanciate a QuickHub DeviceNode
DeviceNode node;

// Instanciate a websocket connection
ArduinoWebSocketsWrapper connectionWrapper;

bool LED_ON = false;

// Here you need to register all properties that the device provides 
void initProperties(JsonObject& properties)
{
  properties["on"] = LED_ON;
}

// The property setter 
void setLedON(JsonObject& properties)
{
  // values of property setters are provided with "val" in the received json object
  bool LED_ON = properties["val"];
  
  // set the LED
  digitalWrite(LED_BUILTIN, !LED_ON);
  
  // confirm property change - Each property change needs to be confirmed!
  node.setProperty("on", LED_ON);
}

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  
  // setup the websocket connection
  node.setAbstractConnection(&connectionWrapper);
  
  // register the initPropertiesCallback (this is mandatory!)
  node.registerInitPropertiesCallback(initProperties);
  
  // give our device a type description
  node.setNodeType("Property example");
  
  // register the property setter
  // The server recognizes a property setter by the fact that it 
  // has the name of a property with a capital initial letter and the prefix "set".
  node.registerRPC("setOn", setLedON);
  
  // This will be the SSID of the AccessPoint that the device will open
  // for initial setup (WiFi credentials, QuickHub Address)
  
  WifiLoader::initWifi("Hello QuickHub IoT", "");
  
  // connect to the address that was received within the WifiManager
  connectionWrapper.autoConnect(4711,4711);
}

void loop() {
  // loop the connection
  Connection::loop();
}
```
