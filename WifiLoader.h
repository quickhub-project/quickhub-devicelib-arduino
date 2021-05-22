#ifndef SDETTINGSLOADER_H
#define SDETTINGSLOADER_H


#include <FS.h>          
#include <ArduinoJson.h> 
#include <WiFiManager.h>
#include <DataStorage.h>

#ifdef ESP32
  #include <SPIFFS.h>
#endif

namespace WifiLoader
{
    void save(char* serverUrl, bool openPortal = false);
    void loadWifiConfigFile();
    void restartAndOpenPortal();   
    char* getServerUrl(); 
    char* getShortId(); 
    void initWifi(char* APName, char* password, bool reset = false, bool openPortal = false); 
}

#endif