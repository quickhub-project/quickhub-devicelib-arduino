
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef ABSTRACT_SLKILL_H
#define ABSTRACT_SLKILL_H

#include "QuickHubDevice.h"
#include <list>

class AbstractSkill
{

public:
   ~AbstractSkill(){ }

  explicit AbstractSkill(const char* skillType, const char* skillID)
  {
      _skillType = skillType;
      _skillID   = skillID;
  }

  virtual void loop() = 0;
  virtual void setup() = 0;
  virtual void initProperties(JsonObject& properties) = 0;
  virtual void registerCallbacks() = 0;

  virtual void setDeviceNode(QuickHubDevice& node)
  {
      _node = &node;
      registerCallbacks();
  }

  virtual const char* getSkillType()
  {
      return _skillType;
  }

  virtual const char* getSkillID()
  {
      return _skillID;
  }

protected:
  QuickHubDevice*    _node = 0;
  void registerRPC(const char* name, std::function<void(JsonObject&)> func)
  {
    char rpcName[120];
    strcat(rpcName, _skillID);
    strcat(rpcName, ".");
    strcat(rpcName, name);
    _node->registerRPC(rpcName, func);
  }

private:
   const char*    _skillType;
   const char*    _skillID;
};

#endif // ABSTRACT_SLKILL_H
