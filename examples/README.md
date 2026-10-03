# HX711_Vidimus - Examples / Példák

This directory contains two distinct application layers designed to cover everything from low-level hardware verification to a complete, production-ready scale implementation.
Ez a könyvtár két különálló alkalmazási szintet tartalmaz, a hardveres ellenőrzéstől a teljes, éles üzemre kész mérleg-megvalósításig.

---

## 1. Dependencies / Függőségek

* **HX711_Vidimus** (This library / Ez a könyvtár) - Core gatekeeper hardware diagnostics.
* **HX711_ADC** by Olav Kallhovd (Required for the Full Control example / Szükséges a Full Control példához). You can install it via the Arduino Library Manager.

---

## 2. Pin Configuration / Lábkiosztás (HX711)

You can assign any digital pins on your microcontroller. Ensure that the pins specified in the code match your physical wiring.
Bármelyik digitalis láb hozzárendelhető a mikrokontrolleren. Ellenőrizd, hogy a kódban megadott lábak egyeznek-e a fizikai bekötéssel.

* **DOUT (Data Out):** Default in examples: `4` (Connects to HX711 DOUT)
* **SCK (Serial Clock):** Default in examples: `3` (Connects to HX711 PD_SCK)

---

## 3. Example Overviews / A példaprogramok áttekintése

### A. BasicCheck.cpp
* **EN:** For advanced developers. Contains *only* the `hx711_vidimus()` gatekeeper function. It does not include any secondary weighing libraries, allowing you to integrate it into your own custom scale architecture or register-level drivers.
* **HU:** Haladó fejlesztőknek. *Csak* a `hx711_vidimus()` kapuőr funkciót tartalmazza külső mérlegkönyvtár nélkül, így könnyen beépíthető bármilyen már meglévő egyedi kódba vagy regiszter-szintű meghajtóba.

### B. full_HX711_control.cpp
* **EN:** For complete automation and calibration. An interactive, production-ready example that walks you through hardware initialization, interactive calibration via the Serial Monitor, zero-bound negative noise filtering, and delta-change tracking (transmits data only on real weight changes). Optimized with raw real-time data flow (`setSamplesInUse(1)`).
* **HU:** Teljes automatizáláshoz és kalibráláshoz. Egy interaktív, éles üzemre kész példa, amely végigvezet a hardver inicializálásán, a Serial Monitoron keresztüli kalibráción, valamint tartalmaz negatív zajlevágást és változás-alapú szűrést is (csak valódi súlyváltozáskor küld adatot). Nyers, valós idejű adatfolyamra optimalizálva (`setSamplesInUse(1)`).
