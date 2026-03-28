# Radio-Frequency-detector

Scans the FM band using a TEA5767 module, detects active stations, and sends the results as an SMS via a SIM900 GSM module.

---

## Hardware

| Component    | Role                    |
|--------------|-------------------------|
| Arduino Uno  | Main microcontroller    |
| TEA5767      | FM receiver / scanner   |
| SIM900       | GSM module for SMS      |

---

## Wiring

### TEA5767 → Arduino Uno (I2C)

| TEA5767 Pin | Arduino Pin |
|-------------|-------------|
| VCC         | 3.3V        |
| GND         | GND         |
| SDA         | A4          |
| SCL         | A5          |

### SIM900 → Arduino Uno (SoftwareSerial)

| SIM900 Pin | Arduino Pin |
|------------|-------------|
| TX         | D7 (RX)     |
| RX         | D8 (TX)     |
| GND        | GND         |
| VCC        | External 5V (2A recommended — **do not power from Arduino 5V pin**) |

> ⚠️ The SIM900 can draw up to 2A during transmission. Use a dedicated power supply or a 5V/2A adapter. Connect its GND to Arduino GND.

---

## Configuration

Open `FM_Scanner_SMS.ino` and update:

```cpp
#define PHONE_NUMBER "+1234567890"   // Replace with the destination number (E.164 format)
#define SIGNAL_THRESHOLD 35          // Raise to filter weak stations (0–15 scale per nibble)
```

You can also adjust the scan range or step:

```cpp
#define SCAN_START   875   // 87.5 MHz
#define SCAN_END     1080  // 108.0 MHz
#define SCAN_STEP    2     // 0.2 MHz resolution
```

---

## Dependencies

All libraries used are built-in to the Arduino IDE — no external libraries needed:

- `Wire.h` — I2C communication with TEA5767
- `SoftwareSerial.h` — UART communication with SIM900

---

## How It Works

1. On startup, the Arduino initializes I2C and the SIM900 modem (`AT`, `AT+CMGF=1` for text mode SMS).
2. It steps through the FM band from 87.5 MHz to 108.0 MHz in 0.2 MHz increments.
3. For each frequency, it tunes the TEA5767 and reads back the signal level from the status registers.
4. Stations with a signal level at or above `SIGNAL_THRESHOLD` are added to a list.
5. After the scan, the full list is sent as a single SMS to `PHONE_NUMBER`.
6. The process repeats every 5 minutes.

---

## Example SMS Output

```
Available FM stations:
89.3 MHz
93.7 MHz
98.1 MHz
103.5 MHz
```

---

## Troubleshooting

| Symptom | Likely Cause |
|--------|--------------|
| No SMS received | SIM card not inserted / not registered on network |
| All frequencies detected | `SIGNAL_THRESHOLD` is too low — raise it |
| No frequencies detected | `SIGNAL_THRESHOLD` is too high, or antenna missing |
| SIM900 not responding | Insufficient power supply; check voltage and current |
| I2C not working | Check SDA/SCL wiring; confirm TEA5767 is on 3.3V |

---

## Notes

- The TEA5767 I2C address is `0x60` (fixed, cannot be changed).
- The SIM900 baud rate is set to `9600` — confirm your module's default with `AT+IPR?`.
- SIM900 must be powered **before** the Arduino sketch starts; add a manual delay or power sequencing if needed.
- An antenna (even a short wire ~75cm) connected to the TEA5767 ANT pin significantly improves reception.

---

## Author

- Udan Marapperuma - [udanmarapperuma@gmail.com](mailto:udan.marapperuma@tum.de)
