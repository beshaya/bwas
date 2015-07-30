#include <bwas.h>
#include <thermistor.h>
#include <pwm.h>
#include <Adafruit_TLC5947.h>

//choose thermistor types for the 8 channels
thermistor_t thermistor_config[8];

//array for reading temperatures
int16_t temperatures[8];

void setup() {

  thermistor_config[0] = THERMISTOR_NTCLG100E2103JB; //1K
  thermistor_config[1] = THERMISTOR_NTCALUG03A103H; //4k7
  thermistor_config[2] = THERMISTOR_NTCALUG03A103H;
  thermistor_config[3] = THERMISTOR_NTCALUG03A103H;
  thermistor_config[4] = THERMISTOR_NTCALUG03A103H;
  thermistor_config[5] = THERMISTOR_NTCALUG03A103H;
  thermistor_config[6] = THERMISTOR_NTCALUG03A103H;
  thermistor_config[7] = THERMISTOR_NTCALUG03A103H;
  Serial.begin(19200);
  //initialize pins
  bwasInit();
  //configure thermistors
  configureThermistors(thermistor_config); 

  //setPwmFrequency(10,2); //set pwm frequency to 15625hz
  //setPwmFrequency(11,2);
  Serial.println("BWAS READY");
}

void readOne(uint8_t channel) {
  printDecimal(readThermistor(channel));
}

void readTouchSwitch(uint8_t channel) {
  if (channel == 1) {
    Serial.print(readTouch1());
  } else if (channel == 2) {
    Serial.print(readTouch2());
  }
}

void readCurrent(uint8_t arg) {
  if (arg == 0) {
    printDecimal(elementCurrent());
  } else if (arg == 1) {
    printDecimal(fanCurrent());
  }
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
        setHeater(arg);
        break;
      case 'c':
        setCooler(arg);
        break;
      case 'H':
        setHeaterFan(arg);
        break;
      case 'C':
        setCoolerFan(arg);
        break;
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
        tlcClear();
        break;
      case 's':
        readTouchSwitch(arg);
        break;
      //I'm out of descriptive letters for heating vs cooling
      case 'I':
        readCurrent(arg);
        break;
      case 'i':
        printDecimal(readIR());
        break;
      default:
        Serial.flush(); 
        return;
    }
    Serial.println(" ");
  }
}
