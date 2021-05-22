
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

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