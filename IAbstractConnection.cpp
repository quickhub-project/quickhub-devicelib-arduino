#include "IAbstractConnection.h"

bool IAbstractConnection::_initialized = false;
IAbstractConnection::socketEvent IAbstractConnection::_eventCallback = nullptr;

IAbstractConnection::IAbstractConnection() 
{
    _initialized = false;
}

void IAbstractConnection::registerEventCallback(socketEvent cb)
{
    _eventCallback = cb;
    _initialized = true;
}

bool IAbstractConnection::isInitialized()
{
     return _initialized;
}