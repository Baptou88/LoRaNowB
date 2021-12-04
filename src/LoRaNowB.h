#ifndef _LORANOWB
#define _LORANOWB
  enum LoRaNowB_State{

  }
  class LoRaNowB
  {
  private:
    /* data */
    String _key;
    HardwareSerial * _serial;
    String * _name;
  public:
    LoRaNowB(/* args */);
    ~LoRaNowB();
    LoraNowB setKey(String key);
    LoRaNowB setSerial(HarwareSerial * serial)

    void work();
  };
  
  
  

#endif