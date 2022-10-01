#include <SPI.h>
#include <LoRa.h>

#ifndef _LORANNOW
#define _LORANNOW
#define LORANOW_DEBUG_STREAM Serial
#ifdef LORANOW_DEBUG_STREAM
#define LORANOW_DEBUG_PRINTLN(...)                 \
    {                                              \
        LORANOW_DEBUG_STREAM.println(__VA_ARGS__); \
    }
#define LORANOW_DEBUG_PRINT(...)                 \
    {                                            \
        LORANOW_DEBUG_STREAM.print(__VA_ARGS__); \
    }
#else
#define LORANOW_DEBUG_PRINT(...)
#define LORANOW_DEBUG_PRINTLN(...)
#endif

#define LORANOW_BUF_SIZE 128
#define LORANOW_PROTOCOL 0x01

enum LoRaNow_State
{
    LN_State_None,
    LN_State_Idle,

    LN_State_Receiving,

    //preparation du packet
    LN_State_TX_INIT,

    //Envoi du packet
    LN_State_TX,

    // Attente du signal onTxDone
    LN_State_TX_Wait,

    // le packet a été envoyé
    LN_State_TX_Done,

};

struct LoRaNowLayer
{
    uint8_t protocol;
	byte dest;
	byte emet;
	uint8_t count;
	uint8_t length;
	uint8_t checksum;
};

class LoRaNowClass: public Stream
{
private:
    bool _gateway = false;

    //State Machine
    LoRaNow_State state = LoRaNow_State::LN_State_None;
    LoRaNow_State nextState;

    //Buffer
    uint8_t payload_buf[LORANOW_BUF_SIZE];
    uint8_t payload_len = 0;
    uint8_t payload_pos = 0;


    byte _deviceId = 0;
    byte _masterId = 0;
    byte _destinataire = 0;


public:
    LoRaNowClass();

    bool Begin(void);
    void Gateway(bool isGatway = false);
    bool isGateway();

    unsigned long _millis = 0;
    unsigned long _delay = 0;
    static void onReceive(int packetSize);
    static void onTxDone();
    void loop();

    //Set ID of this Node
    void setDeviceID(byte _id);
    byte getDeviceID();

    //Set ID of the gatway
    void setMasterID(byte _id);
    byte getMasterID();

    
    void setDestinataire(byte _id);

    void onMessage(void (*cb)(uint8_t *buffer, size_t size));
    // State Machine
    void state_do(LoRaNow_State _state);
    void changeState(LoRaNow_State _state, unsigned long delay = 0);

    virtual size_t write(uint8_t data);
	virtual size_t write(const uint8_t *buffer, size_t size);
    virtual int available();
    virtual int read();
	virtual int peek();
	virtual void flush();

    uint8_t getActiveState();
    uint8_t getNextState();

    void send();
	void receive();
    
    void clear();
	uint8_t *buffer();

private:
    void (*messageCalleback)(uint8_t *buffer, size_t size);

    uint8_t beginPacket();
    uint8_t endPacket();

    int beginDecode();
	int endDecode();

    byte calcCheckSum(byte *packege, byte length);
};

extern LoRaNowClass LoRaNow;

#endif