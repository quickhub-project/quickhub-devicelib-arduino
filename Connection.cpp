#include "Connection.h"
#include <string>
#include "WifiLoader.h"

uint8_t Connection::__lastId                      = 0;
std::map<uint8_t, Connection*> Connection::__map  = std::map<uint8_t, Connection*>();
bool Connection::__connected                      = false;
unsigned long Connection::__lastMillis            = 0;
bool Connection::__enableKeepAliveMessages        = true;
unsigned long Connection::__ackTimer              = 0;
IAbstractConnection* Connection::__connection     = nullptr;

//Ctor
Connection::Connection():
_connected(false),
_id(__lastId++)
{
  __map[_id] = this;
  if(__connected)
    registerHandle();
}

bool Connection::isConnected()
{
  return _connected && __connected;
}

void Connection::resetKeepAliveTimer()
{
  if(__enableKeepAliveMessages)
    __lastMillis = millis();
}

bool Connection::send(JsonObject& json)
{
  if(!_connected)
    return false;

  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();

  msg["uuid"] = _id;
  msg["command"] ="send";
  msg["payload"] = json;

  int len = msg.measureLength();
  char buffer[len+1];
  msg.printTo(buffer, sizeof(buffer));
  uint8_t* arr = (uint8_t*) buffer;
  __connection->sendData(arr, len);
  return true;
}

void Connection::received(JsonObject& json)
{
    resetKeepAliveTimer();
    json.prettyPrintTo(USE_SERIAL);
}

void Connection::registerHandle()
{
  Serial.println("Register Handle...");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["command"] = "connection:register";
  json["uuid"] = _id;
  int len = json.measureLength();
  char buffer[len+1];
  json.printTo(buffer, sizeof(buffer));
  uint8_t* arr = (uint8_t*) buffer;
  __connection->sendData(arr, len);
}

void Connection::connected()
{
  USE_SERIAL.println("Connected!");
}

void Connection::disconnected()
{
  USE_SERIAL.println("Disconnected!");
}

///////////// STATIC PART

void Connection::loop()
{
  __connection->loop();
  
  unsigned long now = millis();
  
  if(__ackTimer != 0 && (now - __ackTimer) > ACK_TIMEOUT)
  {
    #ifdef DEBUG
    Serial.println("TIMEOUT!");
    #endif
    __ackTimer = 0;
    __connection->disconnect();
    __connected = false;
  }

  if(__enableKeepAliveMessages && __connected && (now - __lastMillis > 20000)) // every 20 seconds
  {
    //{\"command\":\"ping\"}
    const char* ping = "{}";
    startAckTimer();
    Serial.println("->ping");
    __connection->sendData( (uint8_t *) ping, 2);
    resetKeepAliveTimer();
  }  
}

void Connection::disconnect()
{
  __connection->disconnect();
}

void Connection::startAckTimer()
{
  if(__ackTimer == 0)
    __ackTimer = millis();
}

void Connection::socketEvent(IAbstractConnection::ConnectionEvent type, uint8_t * payload, size_t length)
{
    switch(type)
    {
      case IAbstractConnection::SOCKEVENT_ERROR:
          __connection->disconnect();
          #ifdef DEBUG
          Serial.println("ERROR");
          #endif
        break;

      case IAbstractConnection::SOCKEVENT_DISCONNECTED:
          #ifdef DEBUG
            Serial.println("Disconnected.");
          #endif

          if(!__connected)
          {
              // wasn't already connected
              // Serial.println("Wasn't connected -> restart and open portal.");
              // WifiLoader::restartAndOpenPortal();
          }
       
          #ifdef ESP32
            ESP.restart();
          #else  
           // ESP.reset();
          #endif


          for (auto connection : __map)
              connection.second->disconnected();
          __connected = false;
          break;

      case IAbstractConnection::SOCKEVENT_CONNECTED:
          #ifdef DEBUG
          USE_SERIAL.println("Connected to server!");
          #endif
          for (auto connection : __map)
              connection.second->registerHandle();
          __connected = true;
          break;

      case IAbstractConnection::SOCKEVENT_DATA:
          #ifdef DEBUG
          Serial.print("Message received: ");
          #endif
          char* data = (char*) payload;
          DynamicJsonBuffer  jsonBuffer;
          JsonObject& root = jsonBuffer.parseObject(data);
          __ackTimer = 0;

          if (!root.success())
          {
              #ifdef DEBUG
              Serial.println("Invalid.");
              #endif
          }

          uint8_t uuid = root["uuid"];
          const char* commandArr = root["command"];
          #ifdef DEBUG
          Serial.println(commandArr);
          #endif
          std::string command(commandArr);


          if(command == "ACK" || command == "pong")
          {    
              #ifdef DEBUG
              Serial.println("<-pong");
              #endif
              break;
          }

          if(command == "ping")
          {
              JsonObject& answer = jsonBuffer.createObject();
              answer["command"] = "pong";
              int len = answer.measureLength();
              char buffer[len+1];
              answer.printTo(buffer, sizeof(buffer));
              uint8_t* arr = (uint8_t*) buffer;
              __connection->sendData(arr, len);
              resetKeepAliveTimer();
          }

          
          if(__map.count(uuid) > 0)
          {
              JsonObject& jsonPayload(root["payload"]);
              Connection* conn = __map[uuid];
              if(command == "send")
                conn->received(jsonPayload);

              if(command == "connection:registered")
              {
                conn->_connected = true;
                conn->connected();
              }
              if(command =="connection:closed")
              {
                conn->_connected = false;
                conn->disconnected();
              }
          }
          break;
      }
}

bool Connection::setAbstractConnection(IAbstractConnection* connection)
{
  if(_initialized)
    return false;

  connection->registerEventCallback(socketEvent);
  __connection = connection;
  _initialized = true;
  return true;
}