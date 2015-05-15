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

#define OVERSAMPLE_MAG 2
#define OVERSAMPLE (1 << OVERSAMPLE_MAG)

static thermistor_t thermistor_config[] = {
  THERMISTOR_NONE, THERMISTOR_NONE, THERMISTOR_NONE, THERMISTOR_NONE,
  THERMISTOR_NONE, THERMISTOR_NONE, THERMISTOR_NONE, THERMISTOR_NONE
};

static uint8_t thermistors_configured = 0;

//these are the addresses of the thermistors in the mux
//the mux had an unreasonable pinout, so these are in an unreasonable order.
static uint8_t cMuxCodes[] = {T0,T1,T2,T3,T4,T5,T6,T7};

static uint8_t enable_green = 0;

static int16_t hotCurrent0;
static int16_t coolCurrent0;

/***************************************************
 * Power Output Functions
 ***************************************************/

void setCooler (uint8_t pwm_val) {
    analogWrite(COOLER, pwm_val);
    analogWrite(HEATER, 0);
}

void coolerOff () {
    analogWrite(COOLER, 0);
}

void setCoolerFan (uint8_t pwm_val) {
    if (pwm_val) {
       digitalWrite(COOLER_FAN, 1);
    } else {
        digitalWrite(COOLER_FAN, 0);
    }
}

void setHeater (uint8_t pwm_val) {
    analogWrite(HEATER, pwm_val);
    analogWrite(COOLER, 0);
}

void heaterOff () {
  analogWrite(HEATER, 0);
}

void setHeaterFan (uint8_t pwm_val) {
    if (pwm_val) {
        digitalWrite(HEATER_FAN, 1);
    } else {
        digitalWrite(HEATER_FAN, 0);
    }
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
    Serial.print(" -> ");
    Serial.println(getThermistorTemp(value, type));
  }
}

/********************************************
 * LED/ring Functions
 ********************************************/

uint8_t readHall () {
    return digitalRead(HALL);
}

void analogWriteRed (uint8_t pwm_val) {
    analogWrite(RED, pwm_val);
}

void analogWriteGreen (uint8_t pwm_val) {
    analogWrite(GREEN, pwm_val);
}

void analogWriteBlue (uint8_t pwm_val) {
    analogWrite(BLUE, pwm_val);
}

uint8_t readTouch1 () {
    return digitalRead(TOUCH1);
}

uint8_t readTouch2 () {
    return digitalRead(TOUCH2);
}

int16_t getCurrent(int16_t raw_adc, int16_t baseline) {
    //convert from adc lsb's to voltage
    int16_t raw_current = (raw_adc-baseline) * (PRECISION/OVERSAMPLE) * 5 / (1<<11 - 1);
    //The ACS715x20 has a gain of 185 mV/A
    int16_t current = raw_current * 185 / 1000;
    return current;
}
    
int16_t heaterCurrent() {
    int16_t raw_adc = 0 ;
    for(int i=0;i < OVERSAMPLE; i++) {
        raw_adc += analogRead(IHEAT);
    }
    return getCurrent(raw_adc, hotCurrent0);
}

int16_t coolerCurrent() {
    int16_t raw_adc = 0;
    for(int i=0;i < OVERSAMPLE; i++) {
        raw_adc += analogRead(ICOOL);
    }
    return getCurrent(raw_adc, coolCurrent0);
}

void calibrateCurrent() {
    coolCurrent0 = 0;
    hotCurrent0 = 0;
    for (int i=0; i < OVERSAMPLE; i++) {
        coolCurrent0 += analogRead(ICOOL);
        hotCurrent0 += analogRead(IHEAT);
    }
}

/************************************************
 * Initialization function
 ************************************************/
uint8_t bwasInit() {
    //initialize Heater and cooler
    pinMode(COOLER, OUTPUT);
    digitalWrite(COOLER, 0);
    pinMode(HEATER, OUTPUT);
    digitalWrite(HEATER, 0);
    //initialize fans
    pinMode(COOLER_FAN, OUTPUT);
    analogWrite(COOLER_FAN, 0);
    pinMode(HEATER_FAN, OUTPUT);
    analogWrite(HEATER_FAN, 0);
    //initialize current sensors
    pinMode(IHEAT, INPUT);
    digitalWrite(IHEAT, 0);
    pinMode(ICOOL, INPUT);
    digitalWrite(ICOOL, 0);

    //initialize sensor pins
    pinMode(MUX_A, OUTPUT);
    pinMode(MUX_B, OUTPUT);
    pinMode(MUX_C, OUTPUT);
    pinMode(MUXOUT, INPUT);
    digitalWrite(MUXOUT, 0);
    analogRead(MUXOUT);
    //initialize LED/hall effect pins
    pinMode(RED, OUTPUT);
    pinMode(BLUE, OUTPUT);
    pinMode(GREEN, OUTPUT);
    digitalWrite(RED, 0);
    digitalWrite(BLUE, 0);
    digitalWrite(GREEN, 0);
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
