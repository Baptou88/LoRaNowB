#include <Arduino.h>
#include "LoRaNowB.h"
#include "LoRa.h"
enum LoraNowB_Mode{
  Master,
  Slave,
};
enum LoRaNowB_State{
  InitSlave,
  Receive,
};
LoRaNowB::LoRaNowB(/* args */)
{
}
  
LoRaNowB::~LoRaNowB()
{
}

void LoRaNowB::begin(){
  now_id = makeId();
  _serial->println('init LoRaNowB with id: ' + String(now_id));
}
void LoRaNowB::setFrequency(int freq){
  _freqency = freq;
}
void LoRaNowB::setKey(String key){
  _key = key;
  // return this*;
}
void LoRaNowB::setSerial(HardwareSerial * serial){
  _serial = serial;
  
}

void LoRaNowB::send(String Outgoing,int dest = -1){
  LoRa.beginPacket();
  LoRa.write(dest); //destinataire
  LoRa.write(0);//local address
  LoRa.write(0); //msgcount
  LoRa.write(Outgoing.length());
  LoRa.endPacket();
}
void LoRaNowB::Mode(LoraNowB_Mode mode){
  switch ( mode)
  {
  case Master:
    _state = Receive;
    _gateway = true;
    break;
  case Slave:
    _state = InitSlave;
    _gateway = false;
    break;
  default:
    break;
  }
}


uint32_t LoRaNowB::makeId()
{
#if defined(ARDUINO_ARCH_AVR)
  uint32_t _id =
      (uint32_t)boot_signature_byte_get(0x13) << 24 |
      (uint32_t)boot_signature_byte_get(0x15) << 16 |
      (uint32_t)boot_signature_byte_get(0x16) << 8 |
      (uint32_t)boot_signature_byte_get(0x17);
  return _id;
#elif defined(ARDUINO_ARCH_ESP8266)
  return ESP.getChipId();
#elif defined(ARDUINO_ARCH_ESP32)
  uint32_t _id = (uint32_t)((uint64_t)ESP.getEfuseMac() >> 16);
  return ((((_id)&0xff000000) >> 24) | (((_id)&0x00ff0000) >> 8) | (((_id)&0x0000ff00) << 8) | (((_id)&0x000000ff) << 24)); // swap bits
#endif
  return 0;
}

void LoRaNowB::work(){
  static ulong lastmillis = 0;
  if (_state = InitSlave)
  {
    if (millis() > lastmillis + 5000)
    {
      _serial->println("demande assosiation");
      send("demande assoc");
    }
    
  }
  if (_state = Receive)
  {
    
  }
  
  
}