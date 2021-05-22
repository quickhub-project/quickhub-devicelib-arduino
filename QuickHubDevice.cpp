
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "AbstractSkill.h"
#include "QuickHubDevice.h"
#include <stdio.h>
#include <string.h>

#include <EEPROM.h>
#include <DataStorage.h>
#include <WifiLoader.h>

#define START_ADDRESS 8

QuickHubDevice::QuickHubDevice() : Connection()
{
}

void QuickHubDevice::setNodeType(const char* nodeType)
{
    _nodeType = nodeType;
}

void QuickHubDevice::connected()
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["command"] = "node:register";
  JsonObject& parameters = jsonBuffer.createObject();
  parameters["type"] = _nodeType;
  JsonArray& rpcArray = jsonBuffer.createArray();

  for( auto callback : _rpcCallbacks )
  {
    JsonObject& cb = jsonBuffer.createObject();
    cb["name"] = callback.first;
    rpcArray.add(cb);
  }
  
  parameters["functions"] = rpcArray;
  parameters["id"] = WiFi.macAddress();
  parameters["sid"] = WifiLoader::getShortId();
  parameters["key"] = getKey();
  Serial.println("authentification key: "+String(getKey()));
  json["parameters"] = parameters;

  if(_initPropertiesCallback)
  {
    JsonObject& properties = jsonBuffer.createObject();
    _initPropertiesCallback(properties);
    properties["ip"] = WiFi.localIP().toString();
    properties["mac"] = WiFi.macAddress();
    for(auto skill : _skills)
    {
       skill->initProperties(properties);
    }
    parameters["properties"] = properties;
  }
  send(json);
}

void QuickHubDevice::disconnected()
{
  Serial.println("Disconnected.");
}

void QuickHubDevice::received(JsonObject& json)
{
  const char* command = json["cmd"];

  if(strcmp(command, "call") == 0)
  {
      JsonObject& parameters = json["params"];
      const char* function = parameters.begin()->key;
      if(_rpcCallbacks.count(function) > 0)
      {
          JsonObject& arguments = parameters.begin()->value;
          callRPC(function, arguments);
      }
  }

  if(strcmp(command, "init") == 0)
  {
    JsonArray& parameters = json["params"].as<JsonArray>();

    for (auto value : parameters)
    {
      const char* function = value["func"];
      if(_rpcCallbacks.count(function) > 0)
      {
            Serial.println(function);
            JsonObject& arguments = value["args"];
            callRPC(function, arguments);
      }
    }
    if(_initFinishedCallback)
      _initFinishedCallback();
  }
  
  if(strcmp(command, "setkey") == 0)
  { 
    uint32_t key = json["params"];
    writeKey(key);
    Serial.println("authentification key: "+String(key));
    return;
  }

  if(strcmp(command, "sleep") == 0)
  {
    JsonObject& parameters = json["params"];
    ulong millis = parameters["time"];
    if(_allowDeepSleep)
    {
      if(_sleepCallback)
      {
        _sleepCallback(millis);
      }
      else
      {
        disconnect();
        ESP.deepSleep(millis);
      }
    }
  }
}

void QuickHubDevice::requestSleep()
{
  _allowDeepSleep = true;
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["cmd"] = "reqsleep";
  send(json);
}

bool QuickHubDevice::sendData(const char* subject, JsonObject& data)
{
  resetKeepAliveTimer();
  startAckTimer();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["cmd"] = "msg";
  JsonObject& parameters = jsonBuffer.createObject();
  parameters["subject"] = subject;
  parameters["data"] = data;
  json["params"] = parameters;
  return send(json);
}

bool QuickHubDevice::sendData(const char* subject)
{
  startAckTimer();
  resetKeepAliveTimer();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["cmd"] = "msg";
  JsonObject& parameters = jsonBuffer.createObject();
  parameters["subject"] = subject;
  json["params"] = parameters;
  return send(json);
}
void QuickHubDevice::registerInitPropertiesCallback(initCallback cb)
{
  _initPropertiesCallback = cb;
}

void QuickHubDevice::registerInitFinishedCallback(initFinishedCallback cb)
{
  _initFinishedCallback = cb;
}

void QuickHubDevice::registerRPC(const char* name, std::function<void(JsonObject&)> cb)
{
    _rpcCallbacks[name] = cb;
}

void QuickHubDevice::registerSkill(AbstractSkill* skill)
{
  //_skills.;
  skill->setDeviceNode(*this);
}

void QuickHubDevice::registerSleepCallback(sleepCallback cb)
{
    _allowDeepSleep = true;
    _sleepCallback = cb;
}

void QuickHubDevice::setAllowDeepsleep(bool allowed)
{
    _allowDeepSleep = allowed;
}

void QuickHubDevice::callRPC(const char* function, JsonObject& arguments)
{
  _rpcCallbacks[function](arguments);
}

bool QuickHubDevice::writeKey(uint32_t key)
{
  return DataStorage::saveAuthKey(key);
}

uint32_t QuickHubDevice::getKey()
{
  return DataStorage::loadAuthKey();
}