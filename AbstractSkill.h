#ifndef ABSTRACT_SLKILL_H
#define ABSTRACT_SLKILL_H

#include "DeviceNode.h"
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

  virtual void setDeviceNode(DeviceNode& node)
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
  DeviceNode*    _node = 0;
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
