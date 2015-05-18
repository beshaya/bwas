/************************************************
 * BWAS - Bottle Warmer Arduino Shield Library
 * Written by Benjamin Shaya
 *
 * This library provides easy access to the hardware on the BWASnano:
 *  - 8x thermistors, multiplex through a MC74HC4051ADG
 *  - 2x High power PWM outputs with ACS715 Current Sensing
 *  - 2x High Power Digital Outputs (Open Drain) with current sensing
 *  - 2x Touch Sensors (AT42QT1010)
 *  - 3x PWM LED signal outputs (for ring) 
 *
 * This library does not provide access to the TMP007 Sensor board;
 * use the Adafruit library for this.
 *
 ************************************************/

#ifndef _BWAS
#define _BWAS 

#include "thermistor.h"
/************************************************
 * Pin Definitions
 ************************************************/
 
//Power outputs
#define COOLER_FAN 13
#define COOLER 11
#define HEATER_FAN 12
#define HEATER 10

//Current sensors
#define IHEAT A7
#define ICOOL A0

//Themistor channels
#define MUXOUT A6
#define MUX_A 9
#define MUX_B 8
#define MUX_C 7
#define T0 3
#define T1 0
#define T2 1
#define T3 2
#define T4 4
#define T5 6
#define T6 7
#define T7 5

//LEDs
#define RED 3
#define BLUE 5
#define GREEN 6
#define HALL A3

//Touch channels
#define TOUCH1 A1
#define TOUCH2 2

uint8_t bwasInit() ;

void coolerOff ();
void setCooler (uint8_t pwm_val);
void setCoolerFan (uint8_t pwm_val);

void heaterOff ();
void setHeater (uint8_t pwm_val);
void setHeaterFan (uint8_t pwm_val);

int16_t heaterCurrent();
int16_t coolerCurrent();

uint8_t readTouch1();
uint8_t readTouch2();

uint8_t configureThermistors (thermistor_t * configs);
uint8_t readAllThermistors (int16_t * temperatures);
int16_t readThermistor (uint8_t index);
int16_t testThermistor (thermistor_t type);

uint8_t readHall ();
void analogWriteRed (uint8_t pwm_val);
void analogWriteBlue (uint8_t pwm_val);
void analogWriteGreen (uint8_t pwm_val);

void calibrateCurrent ();
void printDecimal (int16_t number);
 
#endif
