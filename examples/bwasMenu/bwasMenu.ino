#define ARDUINO_H
#include <bwas.h>
#include <thermistor.h>

//choose thermistor types for the 8 channels
thermistor_t thermistor_config[] = {
  THERMISTOR_MM103J1F, THERMISTOR_USP10982,
  THERMISTOR_NTCALUG03A103H, THERMISTOR_NTCALUG03A103H,
  THERMISTOR_NTCALUG03A103H, THERMISTOR_NTCALUG03A103H,
  THERMISTOR_NONE, THERMISTOR_NONE
};

//array for reading temperatures
int16_t temperatures[8];

void printPrompt() {
  Serial.print("\nOrders, sir? >");  
}

void printMenu() {
  Serial.println("Bottle Warmer Arduino Shield Demo");
  Serial.println("\nMenu:");
  Serial.println("h x     - turn on warmer, set fan to x");
  Serial.println("c x     - turn on cooler, set fan to x");
  Serial.println("t [0-7] - measure temperature probe x");
  Serial.println("t 8     - Measure all temperature probes");
  Serial.println("r x     - Set red LED to x");
  Serial.println("g x     - Set green LED to x");
  Serial.println("b x     - set blue LED to x");  
  Serial.println("o       - all off");
}

void setup() {
  Serial.begin(9600);
  //initialize pins
  bwasInit();
  
  //configure thermistors
  configureThermistors(thermistor_config); 
  
  //give some demo options
  printMenu();
  printPrompt();
}

void printTemp (int16_t temp) {
  Serial.print(temp / PRECISION);
  Serial.print(".");
  Serial.print(temp % PRECISION);
}

void readAll() {
  if (readAllThermistors(temperatures)) {
    for (int i=0; i<8; i++) {
      Serial.print("T");
      Serial.print(i);
      Serial.print(": ");
      printTemp(temperatures[i]);
      Serial.println();
    }
  }
}

void readOne(uint8_t channel) {
  Serial.print("Temperature ");
  Serial.print(channel); 
  Serial.print(" is: ");
  printTemp(readThermistor(channel));
  Serial.println();
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    uint8_t arg = Serial.parseInt();
    Serial.print(cmd);
    Serial.print(" ");
    Serial.println(arg);
    switch (cmd) {
      case 'h':
        if (arg == 0) {
          heaterOff();
          setHeaterFan(0);
          Serial.println("heater off");
          break;
        }
        setHeater(arg);
        setHeaterFan(1);
        Serial.print("heater to ");
        Serial.println(arg);
        break;
      case 'c':
        if (arg == 0) {
          coolerOff();
          setCoolerFan(0);
          Serial.println("cooler off");
          break;
        }
        setCooler(arg);
        setCoolerFan(1);
        Serial.print("cooler to ");
        Serial.println(arg);
        break;
      case 't':
        if (arg == 8) {
           readAll();
        } else {
           readOne(arg);
        }
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
        Serial.println("Diverting all power");
        break;
      default:
        printMenu();
        Serial.flush(); 
    }
    printPrompt();
  }
  delay(100);        // delay in between reads for stability
  
}
