# HX711_Vidimus

An independent, library-agnostic boot-time hardware and load cell integrity verifier for HX711.

Egy független, könyvtár-agnosztikus hardver- és mérőcella-hitelesítő rutin HX711-hez a boot fázisra.

---

## English Description

### The Problem
Standard HX711 Arduino libraries only detect a total module failure (via timeout tracking if DOUT stays HIGH). However, if the load cell wires (A+/A-) are broken, disconnected, or floating, the high-impedance ADC inputs capture ambient noise, causing the converter to output wildly fluctuating, chaotic values. Standard `tare()` routines average this noise out and set it to "zero," masking a critical system failure.

### The Solution (The "Vidimus" Method)
Developed through extensive industrial packaging machine automation experience, **HX711_Vidimus** acts as a hard gatekeeper during the `setup()` sequence *before* any high-level scale library initializes:
1. **Physical Reset & Wake-up Protocol:** Wakes the chip from power-down mode and allows exactly **400 ms** for the analog circuitry, internal voltage reference, and excitation lines (E+/E-) to fully stabilize.
2. **Delta Tracking (Standard Deviation Test):** It takes 10 raw, unfiltered bit-level samples directly from the hardware. 
   - An **intact load cell** yields an extremely low variance (a noise delta below 15,000 counts on the 24-bit scale).
   - A **broken/floating cell** causes the ADC to fluctuate violently, producing a delta in the hundreds of thousands, which immediately triggers the `cella_ok = false` flag.

---

## Magyar Leírás

### A probléma
A szabványos HX711 Arduino könyvtárak csak a modul teljes hardveres leállását képesek detektálni (időtúllépéssel, ha a DOUT láb HIGH szinten ragad). Azonban, ha a mérőcella vezetékei (A+/A-) megszakadnak vagy lebegnek, a nagy impedanciás ADC bemenetek összeszedik a környezeti zajt, amitől az átalakító vadul ugráló, kaotikus értékeket produkál. A standard `tare()` parancsok ezt a zajt is képesek átlagolni és le-nullázni, elfedve a kritikus hibát.

### A megoldás (A "Vidimus" eljárás)
Ipari csomagológépek automatizálási tapasztalatai alapján kifejlesztett **HX711_Vidimus** kapuőrként működik a `setup()` szekvenciában, még a fő mérőkönyvtárak elindulása előtt:
1. **Hardveres ébresztési protokoll:** Felébreszti a chipet az alvó módból, és pontosan **400 ms** időt biztosít az analóg körök, a belső feszültség-referencia és a tápvonalak (E+/E-) teljes stabilizálódására.
2. **Delta-szórásvizsgálat:** 10 nyers, szűretlen mintát vesz közvetlenül a hardver bit-szintű kileptetésével.
   - Egy **ép mérőcella** stabilan tartja a feszültséget, így a minták szórása elenyésző (a delta 15 000 digit alatt marad a 24 bites skálán).
   - Egy **szakadt/lebegő cella** esetén az ADC felbőszül, százezres nagyságrendű zaj-deltát produkálva, ami azonnal kiváltja a hibajelzést.

---

## Usage / Használat

```cpp
#include <HX711_Vidimus.h>

// Global industrial flags
volatile bool modul_ok = false;
volatile bool cella_ok = false;

const int dout_pin = 4;
const int sck_pin = 3;

void setup() {
    // Execute before your main load cell library starts
    hx711_vidimus(dout_pin, sck_pin, modul_ok, cella_ok);
    
    if (modul_ok && cella_ok) {
        // Safe to initialize your main scale library (e.g. HX711_ADC)
    }
}
```
