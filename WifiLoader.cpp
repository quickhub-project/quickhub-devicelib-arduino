
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "WifiLoader.h"
#ifdef ESP32
#include "Wifi.h"
#else
#include "ESP8266WiFiGeneric.h"
#endif
#define ID_LENGTH 4
namespace WifiLoader
{

    bool _openPortal = false;
    char server_url[40];
    char device_id[ID_LENGTH+1];

    void save(char* serverUrl, bool openPortal)
    {
        Serial.println("saving config");
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();
        json["server"] = serverUrl;
        json["restart"] = openPortal;
        DataStorage::writeJSON("/connect.json", json);   
    }

    void generateID()
    {
        // char buffer[ID_LENGTH];
        
        byte mac[6];
        WiFi.macAddress(mac);
        unsigned int seed = 0;
        
        for(int i = 0; i < 6; i++)
        {
            unsigned long currByte = mac[i];
            currByte = currByte << (8 * i);
            seed |=  currByte;
        }
            
        randomSeed(seed);            
        
        for(int i = 0; i < ID_LENGTH; i++)
        {
            byte randomValue = random(0, 35);
            char letter = randomValue + 'A';
            if(randomValue > 25)
                letter = (randomValue - 26) + '0';
            
            device_id[i] = letter;
        }          
        device_id[ID_LENGTH] = '\0';

        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();
        json["id"] = device_id;
        DataStorage::writeJSON("/id.json", json);   
    }

    void loadWifiConfigFile()
    {
        JsonBundle bundle;
        if(DataStorage::loadJSON("/connect.json", bundle))
        {
            strcpy(WifiLoader::server_url, bundle.root()["server"]);
            _openPortal = bundle.root()["restart"];
            Serial.println(server_url);
        }

        JsonBundle bundle2;
        if(DataStorage::loadJSON("/id.json", bundle2))
        {
            strcpy(WifiLoader::device_id, bundle2.root()["id"]);
            Serial.println(device_id);
        }
        else
        {
            Serial.println("loading failed -> generate ID");
            generateID();
        }
    }
    
    bool shouldSaveConfig = false;
    void saveConfigCallback()
    {
        Serial.println("Should save config");
        shouldSaveConfig = true;
    }            
    
    
    char* getShortId()
    {
        return device_id;        
    }

    char* getServerUrl()
    {
        return server_url;
    }

    void restartAndOpenPortal()
    {
        Serial.println("restart, open portal");
        save(server_url, true);
        ESP.restart();
    }

    void initWifi(char* APName, char* password, bool reset, bool openPortal) 
    {
        loadWifiConfigFile();
        
        // WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
        WiFiManager wm;
        if(reset)
        {
            wm.resetSettings();
        }
        //set config save notify callback
        wm.setSaveConfigCallback(saveConfigCallback);

        // setup custom parameters
        // 
        // The extra parameters to be configured (can be either global or just in the setup)
        // After connecting, parameter.getValue() will get you the configured value
        // id/name placeholder/prompt default length

        WiFiManagerParameter custom_server("server", "QuickHub Server", reset ? "" : server_url, 40);

        //add all your parameters here
        wm.addParameter(&custom_server);

        char custom_radio_str[128]; //= "4 C X 3";
        strcpy(custom_radio_str, "<br/><br/><label for='deviceID'>Device ID:</label><h1><center>");
        strcat(custom_radio_str, device_id);
        strcat(custom_radio_str, "</center></h1><h4><center>(Bitte notieren)</center></h4>");
        WiFiManagerParameter custom_field(custom_radio_str); // custom html input
  
        wm.addParameter(&custom_field);
    
   
        //automatically connect using saved credentials if they exist
        //If connection fails it starts an access point with the specified name
        //here  "AutoConnectAP" if empty will auto generate basedcon chipid, if password is blank it will be anonymous
        //and goes into a blocking loop awaiting configuration
        char ap_name[80] = "";
        strcat(ap_name, APName);
        strcat(ap_name, " (");
        strcat(ap_name, device_id);
        strcat(ap_name, ")");
        if (!wm.autoConnect(ap_name, password)) 
        {
            Serial.println("failed to connect and hit timeout");
            delay(3000);
            // if we still have not connected restart and try all over again
            ESP.restart();
            delay(5000);
        }

        // always start configportal for a little while
        if(openPortal || _openPortal)
        {
            save(server_url, false);
            _openPortal = false;
            wm.setConfigPortalTimeout(60);
            wm.startConfigPortal(APName, password);
        }

        //if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");

        //read updated parameters
        strcpy(server_url, custom_server.getValue());

        //save the custom parameters to FS
        if (shouldSaveConfig) 
        {
            save(server_url, false);
            shouldSaveConfig = false;
        }

        Serial.println("local ip");
        Serial.println(WiFi.localIP());
        Serial.println(WiFi.gatewayIP());
        Serial.println(WiFi.subnetMask());
    }
}