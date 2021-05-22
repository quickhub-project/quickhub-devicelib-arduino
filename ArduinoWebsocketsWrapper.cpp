
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "ArduinoWebsocketsWrapper.h"
#include "WifiLoader.h"

bool ArduinoWebSocketsWrapper::__connected = false;
int ArduinoWebSocketsWrapper::__port = 0;
char* ArduinoWebSocketsWrapper::__server = 0;
WebSocketsClient ArduinoWebSocketsWrapper::__webSocket = WebSocketsClient();


void ArduinoWebSocketsWrapper::webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
{
    if(!isInitialized() || _eventCallback == nullptr)
        return;   

    switch(type)
    {
        case WStype_CONNECTED: 
            _eventCallback(SOCKEVENT_CONNECTED, payload, length); 
            __connected = true;
        return;

        case WStype_DISCONNECTED: 
            _eventCallback(SOCKEVENT_DISCONNECTED, payload, length); 
            __connected = false;
        return;

        case WStype_ERROR: _eventCallback(SOCKEVENT_ERROR, payload, length); return;
        case WStype_BIN: _eventCallback(SOCKEVENT_DATA, payload, length); return;
    }
}

void ArduinoWebSocketsWrapper::connectSSL(char* server, int port, String fingerprint)
{
  __server = server;
  __port = port;
  __webSocket.onEvent(webSocketEvent);
  __webSocket.beginSSL(server, port, "/", fingerprint);
}

void ArduinoWebSocketsWrapper::connectSSL( int port, String fingerprint)
{
  connectSSL(WifiLoader::getServerUrl(), port, fingerprint);
  //connectSSL("", port, fingerprint);
}

void ArduinoWebSocketsWrapper::autoConnect(int defaultWSPort, int defaultWSSPort, String fingerPrint)
{
   String server = WifiLoader::getServerUrl();  
  if(server.startsWith("ws://"))
  {
      server = server.substring(5);
      int portPos = server.lastIndexOf(":");
      if(portPos >= 0)
      {
          int port = server.substring(portPos+1).toInt();
          server = server.substring(0, portPos );
          Serial.println("Connect -- Server:"+ server+ "  Port:"+ port);
          char buf[server.length()+1];
          server.toCharArray(buf, server.length()+1);
          connect(buf, port);
          return;
      }
          
      char buf[server.length()+1];
      server.toCharArray(buf, server.length()+1);
      connect(buf, defaultWSPort);
      return;     
  }

  if(server.startsWith("wss://"))
  {
      server = server.substring(6);
  }

  int portPos = server.lastIndexOf(":");
  if(portPos >= 0)
  {
      int port = server.substring(portPos+1).toInt();
      server = server.substring(0, portPos);
      Serial.println("Connect SSL -- Server:"+ server+ "  Port:"+ port);
      char buf[server.length()+1];
      server.toCharArray(buf, server.length()+1);
      connectSSL(buf, port, "");
      return;
  }

  Serial.println("Connect SSL -- Server:"+ server+ "  Port:"+ defaultWSSPort);
  char buf[server.length()+1];
  server.toCharArray(buf, server.length()+1);
  connectSSL(buf, defaultWSSPort, fingerPrint);  
}

void ArduinoWebSocketsWrapper::connect(char* server, int port)
{
  __server = server;
  __port = port;
  __webSocket.onEvent(webSocketEvent);
  __webSocket.begin(server, port);
}

bool ArduinoWebSocketsWrapper::sendData(uint8_t * payload, size_t length)
{
    if(!isInitialized())
        return false;

    return __webSocket.sendBIN(payload, length);
}

void ArduinoWebSocketsWrapper::loop()
{
    __webSocket.loop();
};

bool ArduinoWebSocketsWrapper::isConnected()
{   
    return __connected;
};

void ArduinoWebSocketsWrapper::disconnect()
{
    __webSocket.disconnect();
};