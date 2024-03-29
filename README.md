# The QuickHub Device Library for Arduino

With the help of this library, IoT devices based on the low-cost Espressif microcontrollers can be easily created. With just a few lines of code, sensors and actuators can be connected to a QuickHub server and then controlled from anywhere. 

## Dependencies

- ArduinoJson (version 5.x) by Benoît Blanchon https://github.com/bblanchon/ArduinoJson/tree/5.x
- ArduinoWebsockets by Markus Sattler https://github.com/Links2004/arduinoWebSockets.git
- WifiManager https://github.com/tzapu/WiFiManager.git

## Setup with PlatformIO

Just add the following lines to your ```platformio.ini```

```
lib_deps =  https://github.com/quickhub-project/quickhub-devicelib-arduino.git
            https://github.com/bblanchon/ArduinoJson.git#5.x
            https://github.com/tzapu/WiFiManager.git
            549
            
```

## Example

This example implements a readwrite property which allows to switch the internal LED of the Wemos D1 mini on and off.

```c
#include <Arduino.h>
#include <QuickHubDevice.h>
#include <ArduinoWebsocketsWrapper.h>
#include <WifiLoader.h>

// Instanciate a QuickHub DeviceNode
QuickHubDevice node;

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
  // The value to set the property to is provided in "val" in the passed json object.
  bool LED_ON = properties["val"];
  
  // Set the LED
  digitalWrite(LED_BUILTIN, !LED_ON);
  
  // Confirm property change - Each property change needs to be confirmed!
  node.setProperty("on", LED_ON);
}

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Setup the websocket connection
  node.setAbstractConnection(&connectionWrapper);
  
  // Register the initPropertiesCallback (this is mandatory!)
  node.registerInitPropertiesCallback(initProperties);
  
  // Give our device a type description
  node.setNodeType("Property example");
  
  // Register the property setter callback
  // The server recognizes a property setter by the fact that it 
  // has the name of a property with a capital initial letter and 
  // the prefix "set". If the property has the name "on", the appropriate 
  // setter needs to be registered with "setOn".
  node.registerRPC("setOn", setLedON);
  
  // This will be the SSID of the AccessPoint that the device will open
  // for initial setup (WiFi credentials, QuickHub Address)
  
  WifiLoader::initWifi("Hello QuickHub IoT", "");
  
  // Connect to the address that was received within the WifiManager.
  // You only have to provide the ports for ws and wss:
  connectionWrapper.autoConnect(4711,4711);
}

void loop() {
  // loop the connection
  Connection::loop();
}
```

## Device Setup

If the device cannot connect to a WiFi, it will open a WiFi accesspoint with the apropriate SSID (line 70 in the example). Open your browser and try to browse a random URL. You should be redirected to the setup page.


