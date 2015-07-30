#include <bwas.h>
#include <thermistor.h>
#include <pwm.h>
#include <Adafruit_TLC5947.h>

uint8_t on = 0;
uint8_t heat = 0;

uint8_t last_touch1 = 0;
uint8_t last_touch2 = 0;

void setLEDs(uint8_t on, uint8_t heat) {
  tlcClear();
  if (on) {
    setPWM(GBUTTON2, 100);
    if(heat) {
      setPWM(RBUTTON1, 150);
      setPWM(GBUTTON1, 10);
      setPWM(RED, 150);
      setPWM(GREEN, 10);
    } else {
      setPWM(BBUTTON1, 100);
      setPWM(GBUTTON1, 50);
      setPWM(BLUE, 100);
      setPWM(GREEN, 50);
    }
  } else {
    setPWM(RED, 50);
    setPWM(BLUE, 50);
  }
}

void setup() {
    Serial.begin(19200);
    bwasInit();
    setLEDs(on, heat);
}

void loop() {
  
  uint8_t touch1 = readTouch1();
  uint8_t touch2 = readTouch2();
  uint8_t do_thing = 0;

  if (touch1 && !last_touch1 && on) {
    heat = !heat;
    tlcClear();
    do_thing = 1;
  }
  if (touch2 && !last_touch2) {
    on = !on;
    tlcClear();
    do_thing = 1;
  }

  last_touch1 = touch1;
  last_touch2 = touch2;
  
  if (do_thing) setLEDs(on,heat);
  
  delay(10);
}
