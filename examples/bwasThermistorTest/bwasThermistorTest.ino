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

void setup() {
  Serial.begin(115200);
  //initialize pins
  bwasInit();
  
  //enable the green led
  enableGreen();
  
  //configure thermistors
  configureThermistors(thermistor_config); 
  
  testThermistor(THERMISTOR_NTCALUG03A103H);
}

void loop() {
 delay(100);        // delay in between reads for stability
}
