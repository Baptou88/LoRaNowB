#include <Arduino.h>
#include <LoRaNow.h>


LoRaNowClass::LoRaNowClass()
{
    _gateway = false;
}

/**
 * Starting LoraNow
 * */
bool LoRaNowClass::Begin(void){
    //SPI.begin(SCK,MISO,MOSI,SS);
	LoRa.setPins(SS,RST_LoRa,DIO0);

    if (LoRa.begin(868E6))
    {
        LORANOW_DEBUG_PRINTLN("[ln] Starting LoRa");

        LoRa.receive();

        LoRa.onReceive(LoRaNowClass::onReceive);
        LoRa.onTxDone(LoRaNowClass::onTxDone);

        LoRa.setSpreadingFactor(8);
	    LoRa.setSyncWord(0x12);
	    LoRa.setSignalBandwidth(125E3);

        changeState(LoRaNow_State::LN_State_Idle,5000);
        return true;
    }
    
    LORANOW_DEBUG_PRINTLN("[ln] Starting LoRa Failed");
    return false;
}

void LoRaNowClass::Gateway(bool isGatway ){
    _gateway = isGatway;
}

bool LoRaNowClass::isGateway()
{
    return _gateway;
}

IRAM_ATTR void LoRaNowClass::onReceive(int packetSize){
    if (packetSize ==0)
    {
        Serial.println("eofjn");
        return;
    }
    LoRaNow.changeState(LoRaNow_State::LN_State_Receiving,50);
   
    
}
IRAM_ATTR void LoRaNowClass::onTxDone(){
    LORANOW_DEBUG_PRINTLN("[ln] txDone");
    LoRaNow.changeState(LoRaNow_State::LN_State_TX_Done,6000);
}

void LoRaNowClass::loop()
{
    if (_delay>0)
    {
        //Serial.println(millis()-_millis);
        if (millis()-_millis >= _delay  )
        {
            LORANOW_DEBUG_PRINTLN("[ln] loop change state from " + (String) state + " "+ (String) nextState);
            
            _delay = 0;
            changeState(nextState);
        }
    }
    
}

void LoRaNowClass::setDeviceID(byte _id)
{
    _deviceId = _id;
}

void LoRaNowClass::setMasterID(byte _id)
{
    _masterId = _id;
}

byte LoRaNowClass::getMasterID()
{
    return _masterId;
}

byte LoRaNowClass::getDeviceID()
{
    return _deviceId;
}

void LoRaNowClass::setDestinataire(byte _id)
{
    _destinataire = _id;
}

void LoRaNowClass::onMessage(void (*cb)(uint8_t *buffer, size_t size))
{
    messageCalleback = cb;
}

void LoRaNowClass::state_do(LoRaNow_State st)
{
    LORANOW_DEBUG_PRINTLN("[ln] Executing State: " + String(st));
    byte dest;
    
    switch (st)
    {
    case LoRaNow_State::LN_State_None :
        /* code */
        break;
    case LoRaNow_State::LN_State_Idle :
        LoRa.receive();
        break;
    case LoRaNow_State::LN_State_Receiving :
        LORANOW_DEBUG_PRINTLN("[ln] Receiving");
        
        LoRaNow.beginDecode();

        while (LoRa.available())
        {
            char a = LoRa.read();
            //Serial.println("rt " + String(a));
            LoRaNow.write(a);
        }
        if (LoRaNow.endDecode())
        {
            if (LoRaNow.messageCalleback)
            {
                LoRaNow.messageCalleback((uint8_t *)LoRaNow.buffer(), LoRaNow.available());

            }
           
        }
        
       
        
        
        changeState(LoRaNow_State::LN_State_Idle,2000);
        
        break;

    
    case LN_State_TX_INIT:
        beginPacket();
        changeState(LN_State_TX);
        break;

    case LN_State_TX:
        endPacket();
        changeState(LN_State_TX_Wait);
        break;
    case LN_State_TX_Wait:
        //attente du signal ontxdone
        LORANOW_DEBUG_PRINTLN("[ln] en cours d'envoi")
        break;
    case LN_State_TX_Done:
        //attente du signal ontxdone
        LORANOW_DEBUG_PRINTLN("[ln] le msg a été envoyé")
        changeState(LoRaNow_State::LN_State_Idle);
        break;
    default:
        Serial.println("je passe par là");
        break;
    }
}

void LoRaNowClass::changeState(LoRaNow_State _state, unsigned long delay)
{
    LORANOW_DEBUG_PRINTLN("[ln] change State from to " + (String) _state + " delay " + (String) delay);
    if (delay > 0)
    {
        _delay = delay;
        _millis = millis();
        nextState = _state;
        LORANOW_DEBUG_PRINTLN("[ln]  to " + (String) nextState + " delay " + (String) _delay);
        return ;
    }
    
    state = _state;
    state_do(_state);
    
}

size_t LoRaNowClass::write(uint8_t data)
{
    //LORANOW_DEBUG_PRINTLN("[ln] payload len " + String(payload_len));
    payload_buf[payload_len++] = data;
    //LORANOW_DEBUG_PRINTLN("[ln] payload len " + String(payload_len));
    return 1;
}

size_t LoRaNowClass::write(const uint8_t *buffer, size_t size)
{
    size_t n = 0;
    for (size_t i = 0; i < size; i++)
    {
        n+= write(*(buffer + i));
    }
    return n;
    
}

int LoRaNowClass::available()
{
    return payload_len - payload_pos;
}

void LoRaNowClass::flush()
{
    LORANOW_DEBUG_PRINTLN("[ln] flush ");

    changeState(LoRaNow_State::LN_State_TX_INIT);
}
int LoRaNowClass::read()
{
  return payload_pos < payload_len ? payload_buf[payload_pos++] : -1;
}
int LoRaNowClass::peek()
{
  return payload_pos < payload_len ? payload_buf[payload_pos] : -1;
}
uint8_t LoRaNowClass::getActiveState()
{
    return state;
}

uint8_t LoRaNowClass::getNextState()
{
    return nextState;
}

void LoRaNowClass::send()
{
    flush();
}

void LoRaNowClass::receive()
{
   changeState(LoRaNow_State::LN_State_Idle,20); 
}

void LoRaNowClass::clear()
{
    payload_len = 0;
    payload_pos = 0;
    memset(payload_buf, 0, LORANOW_BUF_SIZE);
}

uint8_t* LoRaNowClass::buffer()
{
    return ((uint8_t *)&payload_buf) + payload_pos;
}

byte LoRaNowClass::calcCheckSum(byte *package, byte length)
{
    byte sum = 0;
    for (int i = 0; i < length; i++)
    {
        sum = package[i] + sum;
    }
    return sum;
}

uint8_t LoRaNowClass::beginPacket()
{
    uint8_t buf[payload_len + 1];
    memcpy((uint8_t *)(buf),payload_buf,payload_len);
    memcpy((uint8_t *)(payload_buf + sizeof(LoRaNowLayer)),buf,payload_len);

    LoRaNowLayer *ln = (LoRaNowLayer*)&payload_buf;
    payload_len += sizeof(LoRaNowLayer);

    ln->protocol = LORANOW_PROTOCOL;
    ln->emet = _deviceId;
    ln->dest = _destinataire;
    ln->count = 0; //TODO change this;
    ln->checksum = 0;

    return 0;

}

uint8_t LoRaNowClass::endPacket()
{
    LoRaNowLayer *ln = (LoRaNowLayer *)&payload_buf;
    if (available())
    {
        ln->length = payload_len - sizeof(LoRaNowLayer);
        ln->checksum = -calcCheckSum(payload_buf, payload_len);
        Serial.println(ln->checksum);
        Serial.println("prot: " + String(ln->protocol));
        Serial.println("emet: " + String(ln->emet));
        LoRa.beginPacket();
        LoRa.write((uint8_t *)LoRaNow.buffer(), LoRaNow.available());
        LoRa.endPacket(true);
    }
    LoRaNow.clear();
    return 0;
}

int LoRaNowClass::beginDecode()
{
    clear();
    return 1;

}

int LoRaNowClass::endDecode()
{
    LORANOW_DEBUG_PRINTLN("[ln] start decode");
    if (payload_buf[0] != LORANOW_PROTOCOL)
    {
      LORANOW_DEBUG_PRINTLN("[ln] Protocol Error");
      clear();
      return 0;
    }
    byte checksum = calcCheckSum(payload_buf, payload_len);
    LORANOW_DEBUG_PRINTLN("[ln] checksum verif:");
    LORANOW_DEBUG_PRINT("[ln] calculated: ");
    LORANOW_DEBUG_PRINT(checksum);
    LORANOW_DEBUG_PRINT(" for ");
    LoRaNowLayer *ln = (LoRaNowLayer *)&payload_buf;
    LORANOW_DEBUG_PRINTLN(ln->checksum);
    
    if (ln->dest == _deviceId)
    {
        //ce message est pour moi
        LORANOW_DEBUG_PRINTLN("[ln] " + (String) ln->dest);
        payload_pos += sizeof(LoRaNowLayer);
        LORANOW_DEBUG_PRINT("[ln] Decode OK - ");
        LORANOW_DEBUG_PRINT(available());
        LORANOW_DEBUG_PRINTLN(" Bytes");
        return 1;

        
    }
    
    
    return 0;
}





LoRaNowClass LoRaNow;