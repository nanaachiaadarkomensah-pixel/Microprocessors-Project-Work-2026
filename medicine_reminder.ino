// Smart Medicine Reminder System
// Biomedial Engineering Group 1

#include <LiquidCrystal.h>

//  Pin Definitions 
const int BUZZER_PIN    = 8;
const int BUTTON_PIN    = 7;
const int GREEN_LED_PIN = 12;  // Lights up when dose is taken
const int RED_LED_PIN   = 11;   // Lights up when dose is missed

// LCD: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//  Pill Schedule 
// Simulates times in seconds from program start 
// Alarm 1 = 5s,  Alarm 2 = 15s,  Alarm 3 = 25s
const int NUM_ALARMS = 3;
const long ALARM_TIMES[NUM_ALARMS]   = {5, 15, 25}; // seconds from start
const char* ALARM_LABELS[NUM_ALARMS] = {"Morning Pill", "Noon Pill", "Night Pill"};

// State Variables 
int  currentAlarm   = 0;
bool alarmActive    = false;
int  missedDoses    = 0;
long alarmStartTime = 0;
const long RESPONSE_WINDOW = 10000; // 10 seconds to respond (use 60000 in real life)

// Setup 
void setup() {
  pinMode(BUZZER_PIN,    OUTPUT);
  pinMode(BUTTON_PIN,    INPUT_PULLUP); // LOW = pressed
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN,   OUTPUT);

  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN,   LOW);

  lcd.begin(16, 2);
  lcd.print("Pill Reminder");
  lcd.setCursor(0, 1);
  lcd.print("  Starting up...");
  delay(2000);
  lcd.clear();
}

//  Main Loop 
void loop() {
  long now = millis() / 1000; // current seconds since start

  // 1. Trigger alarm when scheduled time is reached
  if (currentAlarm < NUM_ALARMS && now >= ALARM_TIMES[currentAlarm] && !alarmActive) {
    triggerAlarm(currentAlarm);
  }

  // 2. While alarm is active, wait for button press or timeout
  if (alarmActive) {
    buzz();

    // Button pressed = dose taken
    if (digitalRead(BUTTON_PIN) == LOW) {
      doseTaken();
    }

    // Time window expired = missed dose
    if (millis() - alarmStartTime > RESPONSE_WINDOW) {
      doseMissed();
    }
  }

  // 3. Idle screen when no alarm
  if (!alarmActive && currentAlarm < NUM_ALARMS) {
    showIdleScreen(currentAlarm);
  }

  // 4. All doses done
  if (currentAlarm >= NUM_ALARMS && !alarmActive) {
    showDoneScreen();
  }

  delay(100);
}

//  Functions 
void triggerAlarm(int index) {
  alarmActive    = true;
  alarmStartTime = millis();

  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN,   LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("!! TAKE PILL !!");
  lcd.setCursor(0, 1);
  lcd.print(ALARM_LABELS[index]);
}

void buzz() {
  // Short beep pattern: on 300ms, off 200ms
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
  delay(200);
}

void doseTaken() {
  alarmActive = false;
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);

  // Green LED: flash 3 times then stay on briefly
  for (int i = 0; i < 3; i++) {
    digitalWrite(GREEN_LED_PIN, HIGH);
    delay(200);
    digitalWrite(GREEN_LED_PIN, LOW);
    delay(150);
  }
  digitalWrite(GREEN_LED_PIN, HIGH); // stay on during message

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Dose Taken!");
  lcd.setCursor(0, 1);
  lcd.print("Good job :)     ");
  delay(2500);

  digitalWrite(GREEN_LED_PIN, LOW);
  lcd.clear();
  currentAlarm++;
}

void doseMissed() {
  alarmActive = false;
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
  missedDoses++;

  // Red LED: flash 3 times then stay on briefly
  for (int i = 0; i < 3; i++) {
    digitalWrite(RED_LED_PIN, HIGH);
    delay(200);
    digitalWrite(RED_LED_PIN, LOW);
    delay(150);
  }
  digitalWrite(RED_LED_PIN, HIGH); // stay on during message

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dose MISSED!");
  lcd.setCursor(0, 1);
  lcd.print("Missed: ");
  lcd.print(missedDoses);
  delay(3000);

  digitalWrite(RED_LED_PIN, LOW);
  lcd.clear();
  currentAlarm++;
}

void showIdleScreen(int index) {
  lcd.setCursor(0, 0);
  lcd.print("Next: ");
  lcd.print(ALARM_LABELS[index]);
  lcd.setCursor(0, 1);
  lcd.print("Missed so far: ");
  lcd.print(missedDoses);
}

void showDoneScreen() {
  lcd.setCursor(0, 0);
  lcd.print(" All doses done ");
  lcd.setCursor(0, 1);
  lcd.print("Missed: ");
  lcd.print(missedDoses);
  lcd.print("       ");
}
