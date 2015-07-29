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
#define COOLER 6
#define HEATER 5

//Current sensors
#define IELEMENT A2
#define IFAN A3

//Themistor channels
#define MUXOUT A6
#define MUX_A 9
#define MUX_B 8
#define MUX_C 7
#define T0 3
#define T1 0
#define T2 1
#define T3 2
#define T4 5
#define T5 4
#define T6 6
#define T7 7

//LEDs

//Touch channels
#define TOUCH1 3
#define TOUCH2 4

//IR
#define IR_T A0
#define IR_P A1

//TLC5947
#define TLC_DATA 11
#define TLC_CLOCK 13
#define TLC_LATCH 10
#define RBUTTON1 6
#define GBUTTON1 7
#define BBUTTON1 8
#define RBUTTON2 9
#define GBUTTON2 10
#define BBUTTON2 11
#define HFAN 12
#define CFAN 13
#define RED 21
#define GREEN 22
#define BLUE 23

//HALL
#define HALL 2
uint8_t bwasInit() ;

void coolerOff ();
void setCooler (uint8_t pwm_val);
void setCoolerFan (uint8_t pwm_val);

void heaterOff ();
void setHeater (uint8_t pwm_val);
void setHeaterFan (uint8_t pwm_val);

int16_t fanCurrent();
int16_t elementCurrent();

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
