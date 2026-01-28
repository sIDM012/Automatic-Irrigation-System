# Automatic Irrigation System
### by sIDM

## Project Overview
The Automatic Irrigation System is designed to maintain optimal soil moisture for plants without constant human involvement. The system automatically activates a water pump when soil moisture drops below a predefined threshold and sufficient water is available in the reservoir.

## Key Features
- Automatic irrigation based on soil moisture level
- Water level monitoring in the reservoir
- Adjustable watering duration
- Fixed interval between watering cycles (6 hours)
- LCD display with real-time system status
- Visual and audible alerts
- Dry-run pump protection
- Manual watering mode
- Settings stored in EEPROM

## Operating Modes
### Automatic Mode
Watering starts automatically when soil moisture is below the threshold, water level is sufficient, and the defined time interval has elapsed.

### Configuration Mode
Activated by holding both buttons for more than 2 seconds. Allows the user to adjust the watering duration.

### Manual Watering Mode
A short simultaneous press of both buttons starts watering manually.

## Safety & Reliability
- Automatic shutdown when water level is too low
- Emergency stop using buttons
- Watering disabled during configuration mode
- Non-blocking operation using millis()

## Expansion Possibilities
- Multiple irrigation zones
- RTC-based scheduling
- Wi-Fi / Bluetooth control
- Data logging
- Percentage-based moisture display

---

**Automatic Irrigation System**  
by sIDM  
*Made in Kazakhstan 🇰🇿*
