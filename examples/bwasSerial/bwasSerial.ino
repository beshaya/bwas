#include <bwas.h>
#include <thermistor.h>
#include <Wire.h>
#include "Adafruit_TMP007.h"

//choose thermistor types for the 8 channels
thermistor_t thermistor_config[] = {
  THERMISTOR_MM103J1F, THERMISTOR_USP10982,
  THERMISTOR_NTCALUG03A103H, THERMISTOR_NTCALUG03A103H,
  THERMISTOR_NTCALUG03A103H, THERMISTOR_NTCALUG03A103H,
  THERMISTOR_NONE, THERMISTOR_NONE
};

//array for reading temperatures
int16_t temperatures[8];

Adafruit_TMP007 tmp007;
uint8_t sensor_found = 0;

void setup() {
  Serial.begin(115200);
  //initialize pins
  bwasInit();
  //enable the green led
  enableGreen();
  //configure thermistors
  configureThermistors(thermistor_config); 
  if (! tmp007.begin(TMP007_CFG_1SAMPLE)) {
    Serial.println("No IR sensor found");
  } else {
    sensor_found = 1;
  }
}

void printTemp (int16_t temp) {
  Serial.print(temp / PRECISION);
  Serial.print(".");
  Serial.print(temp % PRECISION);
}

void readOne(uint8_t channel) {
  printTemp(readThermistor(channel));
}

uint8_t parseNibble(char hex) {
  if ( hex >= '0' && hex <= '9' ) {
    return hex - '0';
  }else if ( hex >= 'A' && hex <= 'F' ) {
    return hex - 'A' + 10;
  }else if ( hex >= 'a' && hex <= 'f' ) {
    return hex - 'a' + 10;
  }else {
    return 0;
  }
}

uint8_t parseHex(char hex1, char hex2) {
    return (parseNibble(hex1) << 4) + parseNibble(hex2);
}

void loop() {
  //wait for 4 bytes - [cmd][hex][hex]\n
  if (Serial.available() >= 4) {
    char cmd = Serial.read();
    if (cmd == '\n') return;
    char hex1 = Serial.read();
    if (hex1 == '\n') return;
    char hex2 = Serial.read();
    if (hex2 == '\n') return;
    char newline = Serial.read();
    if (newline != '\n') return;
    
    uint8_t arg = parseHex(hex1,hex2);
    switch (cmd) {
      case 'h':
        if (arg) heaterOn(); else heaterOff();
        break;
      case 'c':
        if (arg) coolerOn(); else coolerOff();
        break;
      case 'H':
        setHeaterFan(arg);
        break;
      case 'C':
        setCoolerFan(arg);
      case 't':
        readOne(arg);
        break;
      case 'r':
        analogWriteRed(arg);
        break;
      case 'g':
        analogWriteGreen(arg);
        break;
      case 'b':
        analogWriteBlue(arg);
        break;
      case 'o':
        coolerOff();
        heaterOff();
        setCoolerFan(0);
        setHeaterFan(0);
        break;
      case 'i':
        Serial.print(tmp007.readObjTempC());
        break;
      default:
        Serial.flush(); 
        return;
    }
    Serial.println();
  }
}
