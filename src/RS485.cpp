#include "RS485.h"

RS485::RS485(int uart_nr):HardwareSerial(uart_nr){
	setDelay(0,0);
	debugStream = 0;
}

RS485::RS485(const HardwareSerial& serial):HardwareSerial(serial){
	setDelay(0,0);
	debugStream = 0;
}

void RS485::setWriteEnabled(bool enabled) {
	if(pinDE != -1) digitalWrite(pinDE,enabled);
}

void RS485::setReadEnabled(bool enabled){
	if(pinRE != -1) digitalWrite(pinRE,!enabled);
}

void RS485::setMode(uint8_t mode){
	switch(mode){
	case ModeNone:
		setWriteEnabled(false);
		setReadEnabled(false);
	break;
	case ModeTX:
		setWriteEnabled(true);
		setReadEnabled(false);
	break;
	case ModeRX:
		setWriteEnabled(false);
		setReadEnabled(true);
	break;
	}
}

void RS485::begin(size_t baud, uint32_t config, int8_t rxPin, int8_t txPin, int8_t dePin, int8_t rePin, bool readBack){
	pinDE = dePin;
	pinRE = rePin;
	hasReadBack = readBack;
#if defined(ESP32)
	((HardwareSerial*) this)->begin(baud,config,rxPin,txPin);
#else
	((HardwareSerial*) this)->setRx(rxPin);
	((HardwareSerial*) this)->setTx(txPin);
	((HardwareSerial*) this)->begin(baud,config);
#endif
	setDelay(0,0);
	if(pinDE != -1){
		pinMode(pinDE,OUTPUT);
		digitalWrite(pinDE,LOW);
	}
	if(pinRE != -1){
		pinMode(pinRE,OUTPUT);
		digitalWrite(pinRE,HIGH);
	}
}

void RS485::begin(RS485Config conf){
	begin(conf.baudrate, conf.config, conf.pinRX, conf.pinTX, conf.pinDE, conf.pinRE, conf.readBack);
}

void RS485::setDelay(uint32_t start, uint32_t stop){
	startDelay = start;
	stopDelay = stop;
}

void RS485::send(uint8_t *data,uint16_t length){
	beginTransmission();
	write(data,length);
	endTransmission();
}

size_t RS485::write(uint8_t c){
	HardwareSerial::write(c);
	readBackCountTotal += 1;
	readBack();
	return 1;
}

size_t RS485::write(const uint8_t *buffer, size_t size){
	HardwareSerial::write(buffer,size);
	readBackCountTotal += size;
	readBack();
	return size;
}

void RS485::readBack(){
	if(!hasReadBack || !readBackCountTotal) return;
	while(available()){
		read();
		readBackCount++;
	}
}

void RS485::beginTransmission(){
	setMode(ModeTX);
	if(startDelay) delayMicroseconds(startDelay);
	readBackCount = 0;
	readBackCountTotal = 0;
}

void RS485::endTransmission(){
#if defined(CONFIG_IDF_TARGET_ESP32)
	flush(true);
#else
	flush();
#endif
	if(stopDelay) delayMicroseconds(stopDelay);
	setMode(ModeRX);
	if(!hasReadBack || !readBackCountTotal) return;
	uint32_t ms = millis();
	while(readBackCountTotal-readBackCount){
	if(available()){
		read();
		readBackCount++;
		ms = millis();
	}else if(millis()-ms>=100){
		if(debugStream){
			debugStream->print("[Error] 485 Read Back Failed Read/Total:");
			debugStream->print(readBackCount);
			debugStream->print("/");
			debugStream->println(readBackCountTotal);
		}
		break;
	}
	}
	/*Serial.print("[Info] 485 Read Back Read/Total:");
	Serial.print(readBackCount);
	Serial.print("/");
	Serial.println(readBackCountTotal);
	Serial.print("Remains:");
	Serial.println(available());*/
}

void RS485::setDebugStream(Stream &dbStream){
	debugStream = &dbStream;
}