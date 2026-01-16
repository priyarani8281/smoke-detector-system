/*SMOKE DETECTION SYSTEM*/

#include <LiquidCrystal.h>

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(5, 6, 8, 9, 10, 11);

// Hardware pins
int redLed = 3;
int greenLed = 2;
int buzzer = 4;
int smokeSensor = A0;

// Multi-level thresholds
int SAFE_LEVEL = 200;       // Below this = safe
int WARNING_LEVEL = 350;    // Yellow zone - pre-alert
int DANGER_LEVEL = 500;     // Red zone - evacuate

// System variables
int smokeValue = 0;
int maxSmokeRecorded = 0;   // Peak smoke level detected
unsigned long alertStartTime = 0;
bool alertActive = false;

void setup() 
{
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(smokeSensor, INPUT);
  
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  // Startup screen
  lcd.setCursor(0, 0);
  lcd.print("Smart Smoke");
  lcd.setCursor(0, 1);
  lcd.print("Detector v2.0");
  delay(2000);
  
  lcd.clear();
  lcd.print("Calibrating...");
  delay(1000);
  
  // Calibrate baseline
  int baseline = 0;
  for(int i = 0; i < 10; i++) {
    baseline += analogRead(smokeSensor);
    delay(100);
  }
  baseline = baseline / 10;
  
  lcd.clear();
  lcd.print("Baseline: ");
  lcd.print(baseline);
  delay(1500);
  
  Serial.println("=== SMART SMOKE DETECTOR ===");
  Serial.println("Level | Range | Status");
  Serial.println("SAFE  | 0-199 | Green");
  Serial.println("WARN  | 200-349 | Yellow");
  Serial.println("DANGER| 350+ | Red Alert");
  Serial.println();
}

void loop() {
  smokeValue = analogRead(smokeSensor);
  
  // Track maximum smoke level
  if(smokeValue > maxSmokeRecorded) {
    maxSmokeRecorded = smokeValue;
  }
  
  // Status logging
  Serial.print("Smoke: ");
  Serial.print(smokeValue);
  Serial.print(" | Max: ");
  Serial.print(maxSmokeRecorded);
  Serial.print(" | Status: ");
  
  // === LEVEL 3: CRITICAL DANGER ===
  if(smokeValue >= DANGER_LEVEL) {
    Serial.println("CRITICAL ALERT!");
    
    if(!alertActive) {
      alertActive = true;
      alertStartTime = millis();
    }
    
    unsigned long alertDuration = (millis() - alertStartTime) / 1000;
    
    // Fast flashing red
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    
    // Rapid beeping
    tone(buzzer, 2000, 200);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!!! DANGER !!!");
    lcd.setCursor(0, 1);
    lcd.print("Smoke:");
    lcd.print(smokeValue);
    lcd.print(" T:");
    lcd.print(alertDuration);
    lcd.print("s");
    
    delay(300);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EVACUATE NOW!");
    lcd.setCursor(0, 1);
    lcd.print("CALL FIRE DEPT");
    
    delay(300);
  }
  
  // === LEVEL 2: WARNING ===
  else if(smokeValue >= WARNING_LEVEL) {
    Serial.println("WARNING");
    
    alertActive = false;
    
    // Slow flashing
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, HIGH); // Both on = yellow
    
    // Intermittent beep
    if(millis() % 2000 < 1000) {
      tone(buzzer, 1500, 100);
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("! WARNING !");
    lcd.setCursor(0, 1);
    lcd.print("Smoke: ");
    lcd.print(smokeValue);
    
    delay(500);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ventilate Area");
    lcd.setCursor(0, 1);
    lcd.print("Check Source");
    
    delay(500);
  }
  
  // === LEVEL 1: SAFE ===
  else {
    Serial.println("Safe");
    
    alertActive = false;
    
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);
    noTone(buzzer);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Status: SAFE");
    lcd.setCursor(0, 1);
    lcd.print("Air: ");
    lcd.print(smokeValue);
    lcd.print(" Max:");
    lcd.print(maxSmokeRecorded);
    
    delay(500);
  }
}