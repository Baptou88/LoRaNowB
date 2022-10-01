#include <Arduino.h>
#include <heltec.h>
#include <LoRa.h>
#include <LoRaNow.h>

unsigned long millisLed = 0;
bool sendmsg = false;

void isrButton(){
  sendmsg = true;
}

void msgCb(uint8_t *buffer, size_t size){
  
  digitalWrite(25,HIGH);
  millisLed = millis();
  Serial.print("Message: ");
  Serial.write(buffer, size);
  Serial.println("");
  
}
void setup() {
  // put your setup code here, to run once:
  Heltec.begin(true,false,true,true,868E6);
  SPI.begin(SCK,MISO,MOSI,SS);

  LoRaNow.setDeviceID(10);
  LoRaNow.Gateway(true);
  LoRaNow.Begin();
  LoRaNow.onMessage(msgCb);
  
  pinMode(0,INPUT);
  attachInterrupt(0,isrButton,RISING);
}

void loop() {
  // put your main code here, to run repeatedly:

  Heltec.display->clear();

  LoRaNow.loop();

  Heltec.display->drawString(00,5,"id "+(String)LoRaNow.getDeviceID());
  Heltec.display->drawString(40,5,"g "+ (String)LoRaNow.isGateway() + " " +  (String)LoRaNow.getMasterID());

  Heltec.display->drawString(10,20,(String)LoRaNow.getActiveState());
  Heltec.display->drawString(40,20,(String)LoRaNow.getNextState());

  Heltec.display->drawString(10,30,(String)LoRaNow._delay);
  Heltec.display->drawString(40,30,(String)LoRaNow._millis);
 

  Heltec.display->display();

  if (millis() > millisLed + 3000 && millisLed !=0) 
  {
    millisLed = 0;
    digitalWrite(25, LOW);
  }
  
  if (sendmsg)
  {
    sendmsg = false;
    LoRaNow.setDestinataire(11);
    LoRaNow.print("er");
    LoRaNow.send();
  }
  
  delay(50);
}