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

/***************************************************
 * Power Output Functions
 ***************************************************/

 void coolerOn () {
  digitalWrite(COOLER, 1);
  digitalWrite(HEATER, 0);
}

void coolerOff () {
  digitalWrite(COOLER, 0);
}

void setCoolerFan (uint8_t pwm_val) {
  analogWrite(COOLER_FAN, pwm_val);
}

void heaterOn () {
  digitalWrite(HEATER, 1);
  digitalWrite(COOLER, 0);
}

void heaterOff () {
  digitalWrite(HEATER, 0);
}

void setHeaterFan (uint8_t pwm_val) {
  analogWrite(HEATER_FAN, pwm_val);
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
  //pinMode(GREEN, OUTPUT);
  digitalWrite(RED, 0);
  digitalWrite(BLUE, 0);
  //digitalWrite(GREEN, 0);
  pinMode(HALL, INPUT);
  digitalWrite(HALL, 0);
  
  //set thermistor library to shift the correct number of bits
  thermistor_setInputShift(6 - OVERSAMPLE_MAG);
  return 1;//success
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
  delay(10);
  uint16_t value = 0;
  for (int i = 0; i < OVERSAMPLE; i++) {
    value += analogRead(MUXOUT);
  }
  return getThermistorTemp(value, thermistor_config[index]);
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
  if (enable_green) analogWrite(GREEN, pwm_val);
}

void analogWriteBlue (uint8_t pwm_val) {
  analogWrite(BLUE, pwm_val);
}

//Green conflicts with SD card shield.
//if you are not using the SD card, enable green with this function
void enableGreen () {
  enable_green = 1;
  pinMode(GREEN, OUTPUT);
  analogWrite(GREEN, 0);
}
  
