
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef ARDUINOWEBSOCKETSWRAPPER_H
#define ARDUINOWEBSOCKETSWRAPPER_H

#include <WebSocketsClient.h>
#include "IAbstractConnection.h"

class ArduinoWebSocketsWrapper : public IAbstractConnection
{
    public: 
        static void autoConnect(int defaultWSPort, int defaultWSSPort, String fingerPrint = "");
        static void connectSSL(char* server, int port,  String fingerprint = "");
        static void connectSSL(int port,  String fingerprint = "");
        static void connect(char* server, int port);

        bool sendData(uint8_t * payload, size_t length) override;
        void loop() override;  
        bool isConnected() override;
        void disconnect() override;

    private:
        static WebSocketsClient __webSocket;
        static bool __connected;
        static char* __server;
        static int __port;
        static void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
};

#endif