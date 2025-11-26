// ╔══════════════════════════════════════════════════╗
// ║  PASSWORD LOCK with LCD + KEYPAD + SERVO + BUZZER ║
// ║  Works with the latest Wokwi JSON (buzzer on D11) ║
// ╚══════════════════════════════════════════════════╝

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

// ────── CONFIGURATION ──────
#define PASSWORD "1234"          // Change this to your desired password
#define PASS_LENGTH 4
#define BUZZER_PIN 11

// ────── OBJECTS ──────
LiquidCrystal_I2C lcd(0x27, 16, 2);   // I2C address usually 0x27 or 0x3F
Servo servo;

// ────── KEYPAD SETUP ──────
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {8, 7, 6, 5};      // Rows → Arduino pins
byte colPins[COLS] = {12, 4, 3, 2};     // Columns → Arduino pins

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ────── PASSWORD BUFFER ──────
char entered[PASS_LENGTH + 1] = {0};
byte index = 0;

// ────── SETUP ──────
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  
  servo.attach(9);
  servo.write(0);           // Locked position
  delay(500);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);

  beep(100);  // Friendly startup beep
}

// ────── MAIN LOOP ──────
void loop() {
  char key = keypad.getKey();

  if (key) {
    beep(50);                     // Keypress feedback
    lcd.print('*');               // Show asterisk

    if (key == '#' || key == '*') {
      resetPassword();
    }
    else if (index == PASS_LENGTH - 1) {
      entered[index++] = key;
      entered[index] = '\0';
      delay(400);
      checkPassword();
    }
    else {
      entered[index++] = key;
    }
  }
}

// ────── PASSWORD CHECK ──────
void checkPassword() {
  if (strcmp(entered, PASSWORD) == 0) {
    lcd.clear();
    lcd.print(" ACCESS GRANTED ");
    lcd.setCursor(0, 1);
    lcd.print("  Door Unlocked ");
    successMelody();
    servo.write(90);      // Unlock
    delay(5000);
    servo.write(0);       // Lock again
    resetPassword();
  }
  else {
    lcd.clear();
    lcd.print(" ACCESS DENIED  ");
    lcd.setCursor(0, 1);
    lcd.print("  Wrong Password");
    failTone();
    delay(2000);
    resetPassword();
  }
}

// ────── RESET ENTRY ──────
void resetPassword() {
  index = 0;
  memset(entered, 0, sizeof(entered));
  lcd.clear();
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
}

// ────── SOUND FUNCTIONS ──────
void beep(int ms) {
  tone(BUZZER_PIN, 900, ms);
  delay(ms + 10);
  noTone(BUZZER_PIN);
}

void successMelody() {
  tone(BUZZER_PIN, 1200, 150); delay(160);
  tone(BUZZER_PIN, 1400, 150); delay(160);
  tone(BUZZER_PIN, 1800, 400); delay(400);
  noTone(BUZZER_PIN);
}

void failTone() {
  tone(BUZZER_PIN, 400, 600); delay(700);
  tone(BUZZER_PIN, 200, 800); delay(800);
  noTone(BUZZER_PIN);
}