#include <Arduino.h>
#include "LoRaNowB.h"

LoRaNowB::LoRaNowB(/* args */)
{
}
  
LoRaNowB::~LoRaNowB()
{
}

LoRaNowB LoRaNowB::setKey(String key){
  _key = key;
  return this;
}
LoRaNowB LoRaNowB::setSerial(HardwareSerial * serial){
  _serial = serial;
  return this;
}