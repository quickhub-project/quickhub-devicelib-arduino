
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef DEVICENODE_H
#define DEVICENODE_H

#include "Connection.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <functional>


class AbstractSkill;

typedef void (*rpcCallback) (JsonObject&);
typedef void (*initCallback) (JsonObject&);
typedef void (*initFinishedCallback) (void);
typedef void (*sleepCallback) (ulong milliseconds);


class DeviceNode : public Connection
{
  struct StrCompare : public std::binary_function<const char*, const char*, bool> {

  public:
      bool operator() (const char* str1, const char* str2) const
      { return strcmp(str1, str2) < 0; }
  };

  public:
    DeviceNode();

    /*
      Is used to identify the device type
    */
    void setNodeType(const char* nodeType);

    /*
      This function is used to register RPCs or property setter.
      Property setter needs to registered with the prefix "set" followed by the capitalized property name.
      The new value is the field "value" within the JsonObject, given with the callback.

      Note: Don't forget to call "setProperty" to confirm the latest property value after the operation.

      example:

      If you provide an editable property, then I suggest to register a property setter with the following pattern:

      function setVolumeCb(JsonObject& obj)
      {
          //take the new value
          int newVolume = obj["value"];

          // try to set the value
          int newVolume = setVolume(newVolume);

          // Wether setting this value succeeded or not - Don't forget to call "setProperty" with the last value.
          // The server will treat this as an "ack" and update its clients.
          node.setProperty("volume", newVolume);
        }
      }

      node.registerRPC("setDuration", setDurationCb);
    */
    void registerRPC(const char* name, std::function<void(JsonObject&)> callback);

    void registerSkill(AbstractSkill* skill);

    /*
      This callback is called whenever the node (re)connects to the server.
      Register this callback and fill all current device properties into the JsonObject
      reference provided as function argument.

      The server will compare these properties with the shadowed device handle instance.
      If something has changed, the server will set the new values via RPCs.
    */
    void registerInitPropertiesCallback(initCallback cb);

    /*
      This callback will be called after the last init RPC has been called.
      If there are no other jobs left to do, you can use this callback to call "requestSleep()"
    */
    void registerInitFinishedCallback(initFinishedCallback cb);

    /*
      Can be used to register a custom 'sleep(ulong milliseconds)' function.
    */
    void registerSleepCallback(sleepCallback cb);

    /*
      Lets the server know that the device is idle and ready for sleep.
      The sleep intervals will be controlled by the server.
    */
    void requestSleep();

    bool sendData(const char* subject, JsonObject& data);
    bool sendData(const char* subject);

    /*
      Call this function whenever a property of your device has changed.
      It will be directly synchronized with the server.
    */
    template <class T>
    void setProperty(const char* property, T value)
    {
      DynamicJsonBuffer jsonBuffer;
      JsonObject& parameters = jsonBuffer.createObject();
      parameters[property] = value;
      setProperties(parameters);
    };

    void setProperties(JsonObject& parameters)
    {
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["cmd"] = "set";
      json["params"] = parameters;
      send(json);
      startAckTimer();
      resetKeepAliveTimer();
    }
    

    void setAllowDeepsleep(bool allowed);

  protected:
    virtual void connected() override;
    virtual void disconnected() override;
    virtual void received(JsonObject& json) override;
    bool writeKey(uint32_t key);
    uint32_t getKey();

  private:
    const char* _nodeType;
    std::map<const char*, std::function<void(JsonObject&)>, StrCompare> _rpcCallbacks;
    std::list<AbstractSkill*>                       _skills;

    initCallback          _initPropertiesCallback         = 0;
    initFinishedCallback  _initFinishedCallback           = 0;
    sleepCallback         _sleepCallback                  = 0;
    bool                  _allowDeepSleep                 = false;
    void                  callRPC(const char* function, JsonObject& arguments);
};


#endif
