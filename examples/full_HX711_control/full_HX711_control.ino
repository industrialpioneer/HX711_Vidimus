#include <HX711_Vidimus.h>
#include "HX711_ADC.h"

// Global industrial state flags
volatile bool modul_ok = false;
volatile bool cella_ok = false;

// Hardware pins
const int dout_pin = 4;
const int sck_pin = 3;

HX711_ADC LoadCell(dout_pin, sck_pin);

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10); 
    
    Serial.println(F("System booting... Running gatekeeper diagnostics."));
    
    // CALL THE INTEGRITY VERIFIER FUNCTION
    hx711_vidimus(dout_pin, sck_pin, modul_ok, cella_ok);
    
    if (!modul_ok || !cella_ok) {
        Serial.println(F("[CRITICAL] Hardware verification failed. Calibration aborted."));
        while (1) delay(1000); 
    }
    
    Serial.println(F("[ OK ] Hardware verified. Starting interactive calibration tool."));

        // 1. INITIALIZE PRIMARY LOAD CELL LIBRARY
    // Configures pins and establishes structural interface with the hardware
    LoadCell.begin();
    
    // 2. ADC GAIN CONFIGURATION (Channel and Amplification selection)
    // Options: 128 (default, Channel A), 64 (Channel A, lower amplification), 32 (Channel B)
    // We lock in 128 gain on Channel A to achieve maximum resolution (24-bit full scale)
    LoadCell.setGain(128);          
    
    // 3. LOW-LEVEL SOFTWARE BUFFER FILTERING (Crucial for responsive automation)
    // Options: 1 to 16. Higher values average samples over time but introduce signal propagation delay.
    // Setting this to 1 disables the library's internal moving average filter.
    // This guarantees raw, instant real-time data flow, which is mandatory for high-speed industrial dosing.
    LoadCell.setSamplesInUse(1);

    unsigned long stabilizingTime = 2000; 
    boolean _tare = true;
    LoadCell.start(stabilizingTime, _tare);
    
    if (LoadCell.getTareTimeoutFlag()) {
        Serial.println(F("[CRITICAL] Tare timeout. System halted."));
        while (1) delay(1000);
    }

    // INTERACTIVE CALIBRATION PROCEDURE
    Serial.println(F("\n=== INTERACTIVE CALIBRATION ==="));
    Serial.println(F("Step 1: Ensure the scale is completely empty."));
    Serial.println(F("Step 2: Place a known mass (e.g., 1000g) onto the load cell."));
    Serial.println(F("Step 3: Type the EXACT weight(in g) of the object into the Serial Monitor and press Enter."));
    
    // Wait for user input from Serial Monitor
    boolean awaiting_input = true;
    float known_mass = 0;
    
    while (awaiting_input) {
        LoadCell.update(); // Keep the ADC updated in the background
        if (Serial.available() > 0) {
            known_mass = Serial.parseFloat();
            if (known_mass > 0) {
                awaiting_input = false;
            }
        }
        delay(10);
    }
    
    Serial.print(F("Received known mass: ")); Serial.print(known_mass); Serial.println(F(" units."));
    Serial.println(F("Calculating calibration factor..."));
    
    // Perform the mathematical calibration steps based on the library documentation
    LoadCell.refreshDataSet(); 
    float newCalibrationValue = LoadCell.getNewCalibration(known_mass);
    
    Serial.print(F("\n=== CALIBRATION SUCCESSFUL ==="));
    Serial.print(F("Your Calibration Factor is: ")); 
    Serial.println(newCalibrationValue, 4); // 4 decimal precision
    Serial.println(F("Save this value and use it with LoadCell.setCalFactor(value) in your main code."));
    Serial.println(F("================================\n"));
    
    LoadCell.setCalFactor(newCalibrationValue);
}

void loop() {
    // Keep track of the previous weight to detect physical changes
    static int lastWeight = 0; 
    
    // 1. NON-BLOCKING DATA REFRESH
    // Constantly updates the internal dataset via register-level conversion tracking
    if (LoadCell.update()) {
        // Continuous data acquisition in the background
    }
        
        // Convert the float data to a rounded integer value
        int rawWeight = (int)round(LoadCell.getData());
        
        // Zero-bound filter: Cuts off negative drift noise to prevent confusing users
        int weight = (rawWeight < 0) ? 0 : rawWeight; 

        // Delta change filter: Only transmits data if the weight has changed by at least 1 unit.
        // This keeps the Serial buffer clean and optimizes microcontroller execution time.
        if (abs(weight - lastWeight) >= 1) {
            Serial.print(F("Calibrated Weight: "));
            Serial.print(weight); // Printed as a clean, rounded integer
            Serial.println(F(" g"));
            
            // Update the history register
            lastWeight = weight;
        }
    }

