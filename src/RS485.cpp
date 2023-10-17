#include "RS485.h"

RS485::RS485(int uart_nr):HardwareSerial(uart_nr){}

RS485::RS485(const HardwareSerial& serial):HardwareSerial(serial){}

void RS485::enableWrite(bool writeEnabled){
  digitalWrite(pinDE,writeEnabled);
}

void RS485::begin(size_t baud, uint32_t config, int8_t rxPin, int8_t txPin, int8_t dePin){
  pinDE = dePin;
  ((HardwareSerial*) this)->begin(baud,config,rxPin,txPin);
  pinMode(pinDE,OUTPUT);
  digitalWrite(pinDE,LOW);
}

void RS485::send(uint8_t *data,uint16_t length){
  enableWrite(true);
  write(data,length);
  flush(true);
  enableWrite(false);
  uint8_t removeItems = available();
  while(removeItems--) read();
}