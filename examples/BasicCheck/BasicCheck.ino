#include <HX711_Vidimus.h>

// Global industrial state flags - Exact lower-case match
volatile bool modul_ok = false;
volatile bool cella_ok = false;

// Hardware pin configuration - Exact lower-case match
const int dout_pin = 4;
const int sck_pin = 3;

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10); 
    
    Serial.println(F("System booting... Running gatekeeper diagnostics."));
    
    // CALL THE INTEGRITY VERIFIER FUNCTION
    hx711_vidimus(dout_pin, sck_pin, modul_ok, cella_ok);
    
    // Evaluate diagnostic flags
    if (!modul_ok) {
        Serial.println(F("[CRITICAL] HX711 Module error (Not responding)! System halted."));
        while (1) delay(1000); 
    }
    
    if (!cella_ok) {
        Serial.println(F("[CRITICAL] Load cell disconnected or floating! Taring disabled. System halted."));
        while (1) delay(1000); 
    }
    
    // If the gatekeeper clears the system, it is safe to initialize your main scale library
    Serial.println(F("[ OK ] Measurement hardware verified successfully."));
    Serial.println(F("Initializing primary scale library (e.g. HX711_ADC)..."));
}

void loop() {
    // Main execution loop - Measurements are now 100% verified and secure
}
