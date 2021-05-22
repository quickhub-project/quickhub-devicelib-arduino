
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef DATA_STORAGE
#define DATA_STORAGE

#include <FS.h>          
#include <ArduinoJson.h> 
#include <WiFiManager.h>


#ifdef ESP32
  #include <SPIFFS.h>
#endif

struct JsonBundle {
  public:
    bool parse(const char* json) {
      _jsonVariant = _jsonBuffer.parseObject(json);
      _success =  _jsonVariant.as<JsonObject&>().success();
      if(!_success)
        Serial.println("Parsing file failed.");
      return _success;
    }

    const JsonObject& root() const { 
      return _jsonVariant;
    }

    bool success()
    {
        return _success;
    }

  private:
    bool _success = false;
    DynamicJsonBuffer _jsonBuffer;
    JsonVariant _jsonVariant;
};

namespace DataStorage
{
    bool init();
    bool writeJSON(char* name, JsonObject& json);

    bool loadJSON(char* name, JsonBundle& bundle);
    bool saveAuthKey(uint32_t key);
    uint32_t loadAuthKey();
        // bool save(char* name, JsonObject& data);
        // JsonObject&  load(char* name);
};


#endif