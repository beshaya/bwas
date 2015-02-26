/************************************************
 * BWAS - Bottle Warmer Arduino Shield Library
 * Written by Benjamin Shaya
 *
 * This library provides easy access to the hardware on the BWAS:
 *  - 8x thermistors, multiplex through a MC74HC4051ADG
 *  - 4x High power PWM outputs
 *  - 3x LED signal outputs (for ring)
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
 
#define COOLER_FAN 9
#define COOLER 8
#define HEATER_FAN 6
#define HEATER 4

#define MUXOUT 3
#define MUX_A A0
#define MUX_B A1
#define MUX_C A2
#define T0 5
#define T1 7
#define T2 6
#define T3 4
#define T4 2
#define T5 1
#define T6 0
#define T7 3

#define RED 5
#define BLUE 3
#define GREEN 10
#define HALL 2

uint8_t bwasInit() ;

void coolerOff ();
void coolerOn ();
void setCoolerFan (uint8_t pwm_val);

void heaterOff ();
void heaterOn ();
void setHeaterFan (uint8_t pwm_val);

uint8_t configureThermistors (thermistor_t * configs);
uint8_t readAllThermistors (int16_t * temperatures);
int16_t readThermistor (uint8_t index);

uint8_t readHall ();
void analogWriteRed (uint8_t pwm_val);
void analogWriteBlue (uint8_t pwm_val);
void analogWriteGreen (uint8_t pwm_val);
void enableGreen ();


#endif