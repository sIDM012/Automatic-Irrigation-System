// C++ CODE
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- ПИНЫ ----------
const byte sensorPin = A0;
const byte waterLevelPin = A1;
const byte pumpPin = 8;
const byte blueLED = 4;
const byte greenLED = 5;
const byte yellowLED = 7;
const byte redLED = 9;
const byte buzzer = 6;
const byte btnPlus = 2;
const byte btnMinus = 3;

// ---------- НАСТРОЙКИ ----------
const int moistureThreshold = 600;
const int waterThreshold = 100;

const unsigned long wateringInterval = 6UL * 3600UL * 1000UL;
const unsigned long LONG_PRESS = 2000;
const unsigned long BUTTON_DELAY = 200;
const unsigned long LOW_WATER_BEEP = 2000;

int wateringTime = 20;

// ---------- СОСТОЯНИЯ ----------
bool configMode = false;
bool isWatering = false;

unsigned long lastWaterTime = 0;
unsigned long lastButtonTime = 0;
unsigned long wateringStartTime = 0;
unsigned long lastLowWaterBeep = 0;

void setup() {
  pinMode(pumpPin, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(btnPlus, INPUT_PULLUP);
  pinMode(btnMinus, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  EEPROM.get(0, wateringTime);
  if (wateringTime < 5 || wateringTime > 250) wateringTime = 20;

  tone(buzzer, 1000, 200);
}

void loop() {
  unsigned long now = millis();
  int moisture = analogRead(sensorPin);
  int waterLevel = analogRead(waterLevelPin);

  handleButtons(now);
  checkModeAndManual(now, waterLevel);
  autoWatering(now, moisture, waterLevel);
  handleWatering(now, waterLevel);
  updateDisplay(now, moisture, waterLevel);
}

// ---------- АВТОПОЛИВ ----------
void autoWatering(unsigned long now, int moisture, int waterLevel) {
  if (configMode || isWatering) return;
  if (waterLevel < waterThreshold) return;

  if (moisture < moistureThreshold &&
     (now - lastWaterTime > wateringInterval || lastWaterTime == 0)) {
    startWatering(false, now);
  }
}

// ---------- ПОЛИВ ----------
void startWatering(bool manual, unsigned long now) {
  isWatering = true;
  wateringStartTime = now;
  lastWaterTime = now;

  digitalWrite(pumpPin, HIGH);
  digitalWrite(greenLED, HIGH);
  digitalWrite(yellowLED, LOW);

  lcd.setCursor(0, 1);
  lcd.print(manual ? "MANUAL WATERING " : "AUTO WATERING   ");
}

void handleWatering(unsigned long now, int waterLevel) {
  if (!isWatering) return;

  if (waterLevel < waterThreshold ||
     (digitalRead(btnPlus) == LOW && digitalRead(btnMinus) == LOW)) {
    stopWatering();
  }

  if (now - wateringStartTime >= wateringTime * 1000UL) {
    stopWatering();
  }
}

void stopWatering() {
  isWatering = false;
  digitalWrite(pumpPin, LOW);
  digitalWrite(greenLED, LOW);
}

// ---------- КНОПКИ ----------
void handleButtons(unsigned long now) {
  if (!configMode || now - lastButtonTime < BUTTON_DELAY) return;

  if (digitalRead(btnPlus) == LOW) {
    wateringTime = min(wateringTime + 5, 250);
    lastButtonTime = now;
    tone(buzzer, 1500, 30);
  }
  else if (digitalRead(btnMinus) == LOW) {
    wateringTime = max(wateringTime - 5, 5);
    lastButtonTime = now;
    tone(buzzer, 1300, 30);
  }
}

void checkModeAndManual(unsigned long now, int waterLevel) {
  if (digitalRead(btnPlus) == LOW && digitalRead(btnMinus) == LOW) {
    unsigned long start = now;
    while (digitalRead(btnPlus) == LOW && digitalRead(btnMinus) == LOW) {
      if (millis() - start > LONG_PRESS) {
        configMode = !configMode;
        digitalWrite(blueLED, configMode);
        if (!configMode) EEPROM.put(0, wateringTime);
        tone(buzzer, configMode ? 1200 : 800, 300);
        return;
      }
    }
    if (!configMode && waterLevel > waterThreshold) {
      startWatering(true, now);
    }
  }
}

// ---------- ЭКРАН ----------
void updateDisplay(unsigned long now, int moisture, int waterLevel) {
  lcd.setCursor(0, 0);
  lcd.print("M:");
  lcd.print(moisture);
  lcd.print(configMode ? " [SET]" : "      ");

  lcd.setCursor(10, 0);
  lcd.print("W:");
  lcd.print(waterLevel > waterThreshold ? "OK " : "LOW");

  lcd.setCursor(0, 1);

  if (waterLevel < waterThreshold) {
    lcd.print("NO WATER! FILL  ");
    digitalWrite(redLED, (now / 500) % 2);

    if (now - lastLowWaterBeep > LOW_WATER_BEEP) {
      tone(buzzer, 900, 50);
      lastLowWaterBeep = now;
    }
  }
  else if (configMode) {
    lcd.print("Time: ");
    lcd.print(wateringTime);
    lcd.print(" sec      ");
    digitalWrite(redLED, LOW);
  }
  else if (!isWatering && lastWaterTime != 0 && now - lastWaterTime < wateringInterval) {
    unsigned long left = (wateringInterval - (now - lastWaterTime)) / 60000;
    lcd.print("Next: ");
    lcd.print(left / 60);
    lcd.print("h ");
    lcd.print(left % 60);
    lcd.print("m   ");
  }
  else {
    lcd.print("Status: READY   ");
    digitalWrite(redLED, LOW);
  }
}
