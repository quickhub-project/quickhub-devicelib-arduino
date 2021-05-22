
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef IABSTRACTCONNECTION_H
#define IABSTRACTCONNECTION_H

#include <Arduino.h>


 
class IAbstractConnection
{  
    public: 
        enum ConnectionEvent
        {
            SOCKEVENT_ERROR,
            SOCKEVENT_DISCONNECTED,
            SOCKEVENT_CONNECTED,
            SOCKEVENT_DATA
        };

    typedef void (*socketEvent)(IAbstractConnection::ConnectionEvent event, uint8_t * payload, size_t length);
    
    IAbstractConnection();
        
    virtual void registerEventCallback(socketEvent cb);
    static bool isInitialized();

    virtual bool sendData(uint8_t * payload, size_t length) = 0;
    virtual void loop() = 0;
    virtual bool isConnected() = 0;
    virtual void disconnect() = 0;

    private:
        static bool _initialized;
    
    protected:
        static socketEvent  _eventCallback;
};



#endif