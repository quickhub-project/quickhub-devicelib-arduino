
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include "IAbstractConnection.h"

#ifndef ESP32
#include <Hash.h>          
#endif

#include <map>

#define USE_SERIAL Serial
#define ACK_TIMEOUT 10000
#define DEBUG

class Connection
{
  public:
      Connection();
      bool setAbstractConnection(IAbstractConnection* connection);
      bool isConnected();
      bool send(JsonObject& json);
      virtual void disconnect();

  protected:
    virtual void connected();
    virtual void disconnected();
    virtual void received(JsonObject& json);

  private:
    void registerHandle();
    bool _connected;
    bool _initialized = false;
    uint8_t _id;
    

  //static
  public:
      static void connect(char* server, int port);
      
      /*
        This function checks if the server URL starts with ws:// or wss:// and opens the apropriate connection.
        If neither the one nor the other is used, it starts a wss connection with the given ssh fingerprint.
        If the string contains a port (ws://foo:<PORT>) the given port is used. 
        If not, the server connects to the given default port for the apropriate connection type. 
      */
    
      static void connectWifi(char* APName, char* password, bool reset = false, bool openPortal = false);
      
//      static void disconnect();
//      static void reconnect();
      static void loop();

  private:
      static void socketEvent(IAbstractConnection::ConnectionEvent type, uint8_t * payload, size_t length);
      
      static uint8_t __lastId;
      static std::map<uint8_t, Connection*> __map;

      static unsigned long __lastMillis;
      static unsigned long __ackTimer;
      static bool __enableKeepAliveMessages;
      static bool __connected;
      static IAbstractConnection* __connection;

    protected:
      static void startAckTimer();
      static void resetKeepAliveTimer();
};

#endif