

#ifndef _LORANOWB
#define _LORANOWB

#include <Arduino.h>


#define LORANOW_DEFAULT_SS_PIN 18
#define LORANOW_DEFAULT_DIO0_PIN 26
  
  class LoRaNowB
  {
  private:
    /* data */
    String _key;
    HardwareSerial * _serial;
    String * _name;
    LoRaNowB_State _state = InitSlave;
    int _freqency = 868E6;
    bool  _gateway = false;
    uint32_t now_id = 0;
  public:
    LoRaNowB(/* args */);
    ~LoRaNowB();

    
    void Mode(LoraNowB_Mode mode);

    void setKey(String key);
    void setSerial(HardwareSerial * serial);

    void send(String outgoing , int dest = -1);
    void onMessage(void(*callback));
    void setFrequency(int freq);
    void work();
    void begin();
    uint32_t makeId();

  };
  
  
  

#endif