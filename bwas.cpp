/************************************************
 * BWAS - Bottle Warmer Arduino Shield Library
 * Written by Benjamin Shaya
 *
 * This library provides easy access to the hardware on the BWAS:
 *  - 8x thermistors, multiplex through a MC74HC4051ADG
 *  - 4x High power PWM outputs
 *  - 3x LED signal outputs (for ring)
 *  - 1x Hall effect sensor
 * 
 * This library does not provide access to the TMP007 Sensor board;
 * use the Adafruit library for this.
 *
 * Usage:
 *  - Call bwasInit() in Setup()
 *  - Create an array of 8 thermistor settings (see thermistor_config[])
 *        - thermistor_t config[8]
 *  - call configureThermistors (YOUR_THERMISTOR_SETTINGS_ARRAY)
 *  - Lights are controlled via 
 ************************************************/
 
#include "bwas.h"
#include "thermistor.h"
#include "Arduino.h"
#include "Adafruit_TLC5947.h"

#define OVERSAMPLE_MAG 2
#define OVERSAMPLE (1 << OVERSAMPLE_MAG)
#define PWM_MAX_VAL 255

static thermistor_t thermistor_config[] = {
  THERMISTOR_NONE, THERMISTOR_NONE, THERMISTOR_NONE, THERMISTOR_NONE,
  THERMISTOR_NONE, THERMISTOR_NONE, THERMISTOR_NONE, THERMISTOR_NONE
};

static uint8_t thermistors_configured = 0;

//these are the addresses of the thermistors in the mux
//the mux had an unreasonable pinout, so these are in an unreasonable order.
static uint8_t cMuxCodes[] = {T0,T1,T2,T3,T4,T5,T6,T7};


static int16_t fanCurrent0 = 0;
static int16_t elementCurrent0 = 0;

Adafruit_TLC5947 tlc = Adafruit_TLC5947(1, TLC_CLOCK, TLC_DATA, TLC_LATCH);
/***************************************************
 * TLC5947 Functions
 ***************************************************/

void setPWM (uint8_t output, uint8_t pwm_val) {
    int16_t tlc_val = ((int16_t)pwm_val) << 4;
    //bump up those last 4 bits if above half
    if (tlc_val > 2 << 11) tlc_val += (2 << 5) -1;
    tlc.setPWM(output, pwm_val << 4 );
    delay(1);
    tlc.write();
}    

void tlcClear() {
    //clear all outputs
    for(uint16_t i=0; i<8*3; i++) {
        tlc.setPWM(i,0);
    }
    tlc.write();
}

/***************************************************
 * Power Output Functions
 ***************************************************/

void setCooler (uint8_t pwm_val) {
    digitalWrite(COOLER, pwm_val);
}

void coolerOff () {
    digitalWrite(COOLER, 0);
}

void setCoolerFan (uint8_t pwm_val) {
    if (pwm_val > PWM_MAX_VAL) pwm_val = PWM_MAX_VAL;
    setPWM(CFAN, (pwm_val));
    return;
}

void setHeater (uint8_t pwm_val) {
    digitalWrite(HEATER, pwm_val);
}

void heaterOff () {
    digitalWrite(HEATER, 0);
}

void setHeaterFan (uint8_t pwm_val) {
    if (pwm_val > PWM_MAX_VAL) pwm_val = PWM_MAX_VAL;
    setPWM(HFAN, (pwm_val));
    return;
}


uint8_t configureThermistors (thermistor_t * configs) {
  for (int i=0; i<8; i++) {
    if (! IS_THERMISTOR(configs[i])) {
      Serial.println("Error configuring thermistors");
      return 0;
    }
  }
  for (int i=0; i<8; i++) {
    thermistor_config[i] = configs[i];
  }
  thermistors_configured = 1;
}

//reads all thermistors not set to none; 
//must provide a int16_t[8] array for results  
uint8_t readAllThermistors (int16_t * temperatures) {
  if (! thermistors_configured) {
    Serial.println("Must configure thermistors!");
    return 0;
  }
  for (int i=0; i<8; i++) {
    if (thermistor_config[i] != THERMISTOR_NONE) {
      temperatures[i] = readThermistor(i);
    }
  }
  return 1;
}

int16_t readThermistor (uint8_t index) {
  if (index >= 8 || index < 0) return 0x8000;
  if (! thermistors_configured) {
    Serial.println("Must configure thermistors!");
    return 0x8000; //return min int
  }
  //set the mux for the correct pin
  //A is lsb, C is msb
  uint8_t mux_code = cMuxCodes[index];
  digitalWrite(MUX_A, mux_code & 0x1);
  digitalWrite(MUX_B, (mux_code >> 1) & 0x1);
  digitalWrite(MUX_C, (mux_code >> 2) & 0x1);
  //allow time to settle, then read and convert
  delay(40);
  uint16_t value = 0;
  for (int i = 0; i < OVERSAMPLE; i++) {
    value += analogRead(MUXOUT);
    delay(10);
    //Serial.println(value);
  }
  return getThermistorTemp(value, thermistor_config[index]);
}

/**
 * Tests the thermistor library for given type
 * does NOT test the physical thermistor
 */
int16_t testThermistor (thermistor_t type) {
  for (uint16_t j=0; j< 1023; j++) {
    uint16_t value = 0;
    for (int i=0; i < OVERSAMPLE; i++) {
      value += j;
    }
    Serial.print(value);
    int16_t temp = getThermistorTemp(value,type);
    Serial.print(" -> ");
    printDecimal(temp);
    Serial.print(" / ");
    Serial.println(temp);
  }
}

/********************************************
 * LED/ring Functions
 ********************************************/

uint8_t readHall () {
    return digitalRead(HALL);
}

void analogWriteRed (uint8_t pwm_val) {
    setPWM(RED, pwm_val);
    //@todo
}

void analogWriteGreen (uint8_t pwm_val) {
    setPWM(GREEN, pwm_val);
    //@todo
}

void analogWriteBlue (uint8_t pwm_val) {
    setPWM(BLUE, pwm_val);
    //@todo
}

uint8_t readTouch1 () {
    return digitalRead(TOUCH1);
}

uint8_t readTouch2 () {
    return digitalRead(TOUCH2);
}

int16_t getCurrent(int16_t raw_adc, int16_t baseline) {
    //convert from adc lsb's to mV
    int32_t raw_current = (raw_adc-baseline); //16bit int overflows
    raw_current *= (PRECISION/OVERSAMPLE);
    raw_current *= 5000 / (1<<10);
    //The MAX9938TEUK+ has a gain of 25V/V, with a 0.02 ohm resistor gives 500mv/A
    int16_t current = raw_current / 500;
    return current;
}
    
int16_t elementCurrent() {
    int16_t raw_adc = 0 ;
    for(int i=0;i < OVERSAMPLE; i++) {
        raw_adc += analogRead(IELEMENT);
    }
    return getCurrent(raw_adc, elementCurrent0);
}

int16_t fanCurrent() {
    int16_t raw_adc = 0;
    for(int i=0;i < OVERSAMPLE; i++) {
        raw_adc += analogRead(IFAN);
    }
    //Serial.print("raw: ");
    //Serial.print(raw_adc);
    //Serial.print(" baseline: ");
    //Serial.println(coolCurrent0);
    return getCurrent(raw_adc, fanCurrent0);
}

void calibrateCurrent() {
    fanCurrent0 = 0;
    elementCurrent0 = 0;
    for (int i=0; i < OVERSAMPLE; i++) {
        fanCurrent0 += analogRead(IFAN);
        elementCurrent0 += analogRead(IELEMENT);
    }
}

void printDecimal (int16_t temp) {
  if (temp < 0 ){
    Serial.print("-");
    temp = -1*temp;
  }
  Serial.print(temp / PRECISION);
  Serial.print(".");
  uint16_t remainder = temp % PRECISION;
  do{
    Serial.print(remainder * 10 / PRECISION);
    remainder = (remainder * 10) % (PRECISION);
  } while (remainder > 0);
}

/************************************************
 * Initialization function
 ************************************************/
uint8_t bwasInit() {

    tlc.begin();
    tlcClear();

    //initialize Heater and cooler
    pinMode(COOLER, OUTPUT);
    digitalWrite(COOLER, 0);
    pinMode(HEATER, OUTPUT);
    digitalWrite(HEATER, 0);

    //initialize current sensors
    pinMode(IELEMENT, INPUT);
    digitalWrite(IELEMENT, 0);
    pinMode(IFAN, INPUT);
    digitalWrite(IFAN, 0);

    //initialize sensor pins
    pinMode(MUX_A, OUTPUT);
    pinMode(MUX_B, OUTPUT);
    pinMode(MUX_C, OUTPUT);
    pinMode(MUXOUT, INPUT);
    digitalWrite(MUXOUT, 0);
    analogRead(MUXOUT);
    //initialize LED/hall effect pins
    pinMode(HALL, INPUT);
    digitalWrite(HALL, 0);
  
    //initialzie buttons
    pinMode(TOUCH1, INPUT);
    pinMode(TOUCH2, INPUT);
    //set thermistor library to shift the correct number of bits
    thermistor_setInputShift(6 - OVERSAMPLE_MAG);
    calibrateCurrent();
    //todo: up PWM frequency
    return 1;//success
}
