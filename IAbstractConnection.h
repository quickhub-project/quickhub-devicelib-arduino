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