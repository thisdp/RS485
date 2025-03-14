#pragma once
#include "Arduino.h"
#include "HardwareSerial.h"

class RS485Config{
public:
  uint32_t baudrate;
  uint32_t config;
  int8_t pinRX;
  int8_t pinTX;
  int8_t pinDE;
  int8_t pinRE;
  bool readBack;
};

class RS485 : public Stream{
public:
  constexpr static uint8_t ModeNone = 0x00;
  constexpr static uint8_t ModeTX = 0x01;
  constexpr static uint8_t ModeRX = 0x02;
  RS485(HardwareSerial& hwSerial);
  void setDebugStream(Stream &dbStream);
  void begin(size_t baud, uint32_t config = SERIAL_8N1, int8_t rxPin=-1, int8_t txPin=-1, int8_t dePin=-1, int8_t rePin = -1, bool readBack = false);
  void begin(RS485Config conf);
  void setDelay(uint32_t start, uint32_t stop);
  void setWriteEnabled(bool enabled);
  void setReadEnabled(bool enabled);
  void setMode(uint8_t mode);
  void send(uint8_t *data,uint16_t length);
  void beginTransmission();
  size_t write(uint8_t);
  size_t write(const uint8_t *buffer, size_t size);
  inline int available() { return serial->available(); }
  inline int read() { return serial->read(); }
#if defined(CONFIG_IDF_TARGET_ESP32)
  inline void flush() { serial->flush(true); }
#else
  inline void flush() { serial->flush(); }
#endif
  inline int peek() { return serial->peek(); }
  void endTransmission();
  void readBack();
  uint32_t readBackCountTotal;
  uint32_t readBackCount;
  uint32_t startDelay;
  uint32_t stopDelay;
  bool hasReadBack;
  int8_t pinDE; // Write Enable Pin
  int8_t pinRE; // Read Enable Pin
  int8_t pinTx; // TX Pin
  int8_t pinRx; // RX Pin
  union{
    struct{
      uint8_t readBackFailed:1;
      uint8_t reserved:7;
    };
    uint8_t errorFlag;
  };
  Stream *debugStream;
private:
  HardwareSerial *serial;
};