
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "DataStorage.h"

namespace DataStorage
{
    bool _init = false;
    
    bool init()
    {
        if(_init)
            return true;
        
        #ifdef ESP32
            _init = (SPIFFS.begin(true));  // format if mounting fails
        #else
            _init = SPIFFS.begin();
            if(!_init)
                Serial.println("Init failed!");
        #endif
        return _init;
    }
    bool writeJSON(char* name, JsonObject& json)
    {
        if(!init())
            return false;

        File configFile = SPIFFS.open(name, "w");
        if (!configFile) 
        {
            Serial.println("failed to open config file for writing");
            return false;
        }

        json.printTo(configFile);
        configFile.close();
        return true;
    }

    bool loadJSON(char* name, JsonBundle& bundle)
    {
        if(!init())
        {
            Serial.println("Unable to init library");
            return false;
        }
            
        if (!SPIFFS.exists(name))
        {
            
            Serial.println("File doesnt exist");
            return false;
        }

        File configFile = SPIFFS.open(name, "r");
        if (!configFile) 
        {
            Serial.println("Unable to open file");
            return false;                     
        }
            
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        bundle.parse(buf.get());
        configFile.close();
        return bundle.success();      
    }
        bool saveAuthKey(uint32_t key)
        {            
            DynamicJsonBuffer jsonBuffer;
            JsonObject& json = jsonBuffer.createObject();
            json["key"] = key;
            return writeJSON("/auth.json", json);
        }

        uint32_t loadAuthKey()
        {
            JsonBundle bundle;
            if(loadJSON("/auth.json", bundle))
            {
                return bundle.root()["key"];
            }
        }    


        // bool save(char* name, JsonObject& data);
        // JsonObject&  load(char* name);
};