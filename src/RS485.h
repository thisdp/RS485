#pragma once
#include "HardwareSerial.h"

class RS485 : public HardwareSerial{
public:
	RS485(int uart_nr);
  RS485(const HardwareSerial& serial);
  void begin(size_t baud, uint32_t config = SERIAL_8N1, int8_t rxPin=-1, int8_t txPin=-1, int8_t dePin=-1);
  void enableWrite(bool writeEnable);
  void send(uint8_t *data,uint16_t length);
  int8_t pinDE;
  int8_t pinTx;
  int8_t pinRx;
};