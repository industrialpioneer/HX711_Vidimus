#ifndef HX711_VIDIMUS_H
#define HX711_VIDIMUS_H

#include <Arduino.h>

/**
 * @brief HX711 Hardware and Load Cell Integrity Verifier (Vidimus)
 *
 * A library-agnostic guard function running during setup to validate HX711 module
 * responsiveness and diagnose load cell wire breakage or floating inputs using standard
 * deviation (delta tracking) before high-level scale libraries initialize.
 *
 * @param dout_pin   The HX711 DOUT (data) pin
 * @param sck_pin    The HX711 PD_SCK (clock) pin
 * @param modul_ok   Global flag reference indicating module responsiveness
 * @param cella_ok   Global flag reference indicating load cell wiring integrity
 */
void hx711_vidimus(int dout_pin, int sck_pin, volatile bool& modul_ok, volatile bool& cella_ok);

#endif // HX711_VIDIMUS_H
