
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

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