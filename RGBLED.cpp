#include "RGBLED.h"
#include <Arduino.h>
#include <FastLED.h>

RGBLED::RGBLED() {
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(_leds, NUM_LEDS); // WS2812B is the chipset of the LED driver, see FastLED library for other supported chipsets
  FastLED.setCorrection(TypicalLEDStrip);  // https://github.com/FastLED/FastLED/wiki/FastLED-Color-Correction
  FastLED.setBrightness(255);  // set to maximum, fine adjustment of brightness is done with setIntensity
}

void RGBLED::show() {
  FastLED.show();
}

void RGBLED::set_gamma(bool enabled) {
  _gamma = enabled;
}

uint16_t RGBLED::gamma_correction(uint16_t value) {
  // access content from PROGMEM: https://playground.arduino.cc/Main/PROGMEM
  uint16_t new_value = pgm_read_word(gamma_correction_2_2 + value);
  return new_value;
}

void RGBLED::all_off() {
  // Switch all LEDs off
  for (int i=0; i<NUM_LEDS; i++) _leds[i].setRGB(0, 0, 0);
  show();
}

struct RGB RGBLED::hv2rgb(uint16_t h, uint16_t v) {
  /* Converts "hue" (h) and "value" (v) (from hsv) to RGB. Assuming maximum "sat" (s).
  H is in range [0,3071], V is in range [0, 1023]
  */
  struct RGB rgb;
  uint16_t r, g, b;

  if (h>3071) h=3071;
  if (v>1023) v=1023;

  uint8_t t = h/1023; // In which "third" are we? (t=0, 1, 2)
  uint16_t p = h-t*1023; // Position within third

  if (!rainbow) {
    switch(t) {
      case 0:
        r = 1023-p;
        g = p;
        b = 0;
        break;
      case 1:
        r = 0;
        g = 1023-p;
        b = p;
        break;
      case 2:
        r = p;
        g = 0;
        b = 1023-p;
        break;
    }
  }

  rgb.r = r;
  rgb.g = g;
  rgb.b = b;
  return rgb;
}

void RGBLED::setRGB(uint16_t r, uint16_t g, uint16_t b) {
  /*
  r, g, b range: 0 .. 1023 (4x 8 bit LED)
  */

  if (_gamma==true) {
    r = gamma_correction(r);
    g = gamma_correction(g);
    b = gamma_correction(b);
  }

  // red LED
  uint8_t n = r/256;  // integer division => gives number of LEDs to be switched on
  uint8_t rem = r%256;  // modulo => gives the level for the "last" LED
  for (int i=0; i<n; i++) _leds[i].r = 255;
  _leds[n].r = rem;

  // green LED
  n = g/256;
  rem = g%256;
  for (int i=0; i<n; i++) _leds[i].g = 255;
  _leds[n].g = rem;

  // blue LED
  n = b/256;
  rem = b%256;
  for (int i=0; i<n; i++) _leds[i].b = 255;
  _leds[n].b = rem;

  show();
}

void RGBLED::setIntensity(uint16_t intensity) {
  _intensity = intensity;
}

void RGBLED::setHue(uint8_t hue) {
  _hue = hue;
}

void RGBLED::updateHSV() {
  setHV(_hue, _intensity);
  show();
}

void RGBLED::setHV(uint8_t hue, uint16_t val) {
  /* Set Hue and Value (Brigthness) in HSV scale. Saturation is set to maximum.

  uint8_t hue : Hue value (0 .. 255)
  uint16t val : Brigthness (0 .. 1023)
  */

  if (_gamma==true) {
    val = gamma_correction(val);
  }

  for (int i=0; i<NUM_LEDS; i++) _leds[i].setRGB(0,0,0); // reset all LEDs to 0

  uint8_t n = val/256;  // integer division => gives number of LEDs to be switched on
  uint8_t rem = val%256;  // modulo => gives the level for the "last" LED
  for (int i=0; i<n; i++) _leds[i].setHSV(hue, 255, 255);
  _leds[n].setHSV(hue, 255, rem);

  show();
}

void RGBLED::sweep_intensity_with_delay(uint16_t intensity_start, uint16_t intensity_stop, uint16_t di, uint16_t dt) {
  if (intensity_stop > intensity_start) {
    for (uint16_t i=intensity_start; i<intensity_stop; i+=di) {
      setHV(_hue, i);
      delay(dt);
    }
  } else {
    for (uint16_t i=intensity_start; i>intensity_stop; i-=di) {
      setHV(_hue, i);
      delay(dt);
    }
  }
}

void RGBLED::sweep_with_delay(uint16_t hue_start, uint16_t hue_stop, uint16_t dt) {
/*
Perform color sweep from hue_start to hue_stop. "dt" is the delay time between steps (in ms).
*/

// Use built-in setHSV method. Limited to 8 bit resolution
  if (hue_stop > hue_start) { // scan from red to blue
    for (uint16_t h=hue_start; h<hue_stop; h++) {
      setHV(h, _intensity);
      delay(dt);
    }
  } else {    // scan from blue to red
    for (uint16_t h=hue_start; h>hue_stop; h--) {
      setHV(h, _intensity);
      delay(dt);
    }
  }


/*
// Use hue-sweep and the full resolution (4 outputs for each LED)
// => need to convert from HSV to RGB
uint16_t dt = sweep_time/(hue_stop-hue_start);
  struct RGB rgb;
  for (uint16_t h=hue_start; h<=hue_stop; h++) {
    rgb = hsv2rgb(h, 1023, val);
    setRGB(rgb.r, rgb.g, rgb.b);
    delay(dt);
  }
  */


  /*
  // Direct setting of RGB values
  struct RGB rgb;
  for (uint16_t h=hue_start; h<hue_stop; h++) {
    rgb = hv2rgb(h, val);
    setRGB(rgb.r, rgb.g, rgb.b);
    delay(dt);
  }
  */

}

// ******************* Sweep functions ****************************
// For the use in an external state machine

void RGBLED::update_sweep_status(uint16_t hue_start, uint16_t hue_stop, uint16_t dh, uint16_t dt) {
  sweep_status.hue_start = hue_start;
  sweep_status.hue_stop = hue_stop;
  sweep_status.dh = dh;
  sweep_status.dt = dt;
  sweep_status.hue = hue_start;
  sweep_status.enabled = false;
  sweep_status.last_update_time = millis();
}

void RGBLED::enableSweep(bool enabled) {
  sweep_status.enabled = enabled;
  sweep_status.last_update_time = millis();
}

void RGBLED::resetSweep() {
  sweep_status.hue = sweep_status.hue_start;
  sweep_status.last_update_time = millis();
}

float RGBLED::progress() {
  /* Returns the sweep progress in %
  */
  uint16_t delta_hue = sweep_status.hue_stop - sweep_status.hue_start;
  return ((float)sweep_status.hue-(float)sweep_status.hue_start)/((float)delta_hue)*100;
}

uint8_t RGBLED::updateSweep(uint8_t hue_offset) {
  /*
  Returns the following:
  0: Sweep is disabled
  1: Hue was updated (i.e. a sweep step was performed)
  2: End of sweep reached
  3: Nothing happened (next dt not yet reached)
  */
  if (sweep_status.enabled) {
    uint32_t current_time = millis();
    if (current_time-sweep_status.last_update_time > sweep_status.dt) {  // time to update
      sweep_status.last_update_time = current_time;
      uint16_t hue = sweep_status.hue + sweep_status.dh;

      if (hue <= sweep_status.hue_stop) {  // sweep stop not yet reached => increment hue
        //struct RGB rgb = _rgbled.hv2rgb(hue, 1023);
        //_rgbled.setRGB(rgb.r, rgb.g, rgb.b);
        setHue(hue + hue_offset);
        updateHSV();

        sweep_status.hue = hue;
        return 1;
      } else {  // end of sweep reached => disable sweep
        sweep_status.enabled = false;
        return 2;
      }
    } return 3;
  }
  return 0;
}
