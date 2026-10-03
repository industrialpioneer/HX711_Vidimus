#include "HX711_Vidimus.h"

void hx711_vidimus(int dout_pin, int sck_pin, volatile bool& modul_ok, volatile bool& cella_ok) {
    modul_ok = false;
    cella_ok = false;

    const unsigned long TIMEOUT_MS = 150;
    const int MINTA_SZAM = 10;

    // Industrial threshold verified by extensive testing
    // Intact cell: 50-80 | Broken/floating cell: 491-2000 | Missing module: 0
    const long MAX_ELVART_DELTA = 200;

    pinMode(dout_pin, INPUT);
    pinMode(sck_pin, OUTPUT);

    // 1. HARDWARE RESET & WAKE-UP
    digitalWrite(sck_pin, HIGH);
    delayMicroseconds(60);
    digitalWrite(sck_pin, LOW);

    // 2. ANALOG TRANSIENT STABILIZATION TIME
    // Gives 500 ms for the internal reference (VBG) and excitation lines (E+/E-) to settle
    delay(500);

    // 3. BASE MODULE RESPONSIVENESS CHECK
    unsigned long start_millis = millis();
    while (millis() - start_millis < TIMEOUT_MS) {
        if (digitalRead(dout_pin) == LOW) {
            modul_ok = true;
            break;
        }
    }

    // 4. LOAD CELL DATA SAMPLING WITH ADC SYNCHRONIZATION
    long min_ertek = 8388607;
    long max_ertek = -8388608;

    for (int m = 0; m < MINTA_SZAM; m++) {
        unsigned long minta_timeout = millis();
        while (digitalRead(dout_pin) == HIGH) {
            if (millis() - minta_timeout > TIMEOUT_MS) break;
        }

        // Explicit loop initialization to prevent shifting drifts
        unsigned long bit_csomag = 0;

        for (int i = 0; i < 24; i++) {
            digitalWrite(sck_pin, HIGH);
            delayMicroseconds(5);

            bit_csomag = bit_csomag << 1;
            if (digitalRead(dout_pin)) bit_csomag++;

            digitalWrite(sck_pin, LOW);
            delayMicroseconds(5);
        }

        // 25th clock pulse to lock in Channel A, 128 Gain
        digitalWrite(sck_pin, HIGH);
        delayMicroseconds(5);
        digitalWrite(sck_pin, LOW);
        delayMicroseconds(5);

        // Two's complement sign extension (24-bit to signed long)
        if (bit_csomag & 0x800000) bit_csomag |= 0xFF000000;
        long nyers_minta = (long)bit_csomag;

        if (nyers_minta < min_ertek) min_ertek = nyers_minta;
        if (nyers_minta > max_ertek) max_ertek = nyers_minta;

        // Settle time between consecutive samples
        delay(20);
    }

    long delta = max_ertek - min_ertek;

    // Module hardware check rule: If delta is exactly 0, the IC is missing from the socket!
    if (delta == 0) {
        modul_ok = false;
    }

    // Final validation rule
    if (modul_ok && delta > 0 && delta <= MAX_ELVART_DELTA) {
        cella_ok = true;
    }
    else {
        cella_ok = false;
    }

    // INDUSTRIAL DIAGNOSTIC LOG
    Serial.println(F("--- VIDIMUS DIAGNOSTIC ---"));
    Serial.print(F("Module software status: ")); Serial.println(modul_ok ? F("RESPONDING") : F("NOT RESPONDING"));
    Serial.print(F("Load cell software status: ")); Serial.println(cella_ok ? F("RESPONDING") : F("NOT RESPONDING"));
    Serial.print(F("Measured Delta value: ")); Serial.println(delta);
    if (cella_ok) {
        Serial.println(F("Notice: The load cell still requires calibration! (examples/full_HX711_control)"));
    }
    Serial.println(F("--------------------------"));
}
