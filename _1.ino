#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Define pins
const int pirPin = 2;        // PIR motion sensor
const int buzzerPin = 3;     // Buzzer
const int ledPin = 4;        // LED indicator

// LCD settings (may need to change I2C address depending on your module)
LiquidCrystal_I2C lcd(0x20, 16, 2);

// Keypad settings
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {5, 6, 7, 8};       // Connect to the row pins of the keypad
byte colPins[COLS] = {9, 10, 11, 12};    // Connect to the column pins of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String correctPIN = "1234";     // Default system PIN code
String enteredPIN = "";         // Stores user input

bool systemArmed = false;       // Indicates if system is armed
bool alarmTriggered = false;    // Indicates if alarm is currently triggered

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  lcd.init();           // Initialize the LCD
  lcd.backlight();      // Turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("Enter PIN:");
}

void loop() {
  char key = keypad.getKey();

  // Handle keypad input
  if (key) {
    if (key == '#') {
      // Check if entered PIN is correct
      if (enteredPIN == correctPIN) {
        systemArmed = !systemArmed;      // Toggle system armed state
        alarmTriggered = false;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(systemArmed ? "System Armed" : "System Disarmed");
        Serial.println(systemArmed ? "System is now ARMED" : "System is now DISARMED");

        tone(buzzerPin, 1000, 200);      // Short beep for success
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wrong PIN!");
        Serial.println("Incorrect PIN entered");
        tone(buzzerPin, 300, 500);       // Error tone
      }

      enteredPIN = "";                   // Reset input
      delay(1000);
      lcd.clear();
      lcd.print("Enter PIN:");
    }
    else if (key == '*') {
      // Clear current PIN input
      enteredPIN = "";
      lcd.clear();
      lcd.print("Enter PIN:");
    }
    else {
      // Add key to PIN input
      enteredPIN += key;
      lcd.setCursor(0, 1);
      lcd.print(enteredPIN);
    }
  }

  // Trigger alarm if motion is detected while system is armed
  if (systemArmed && digitalRead(pirPin) == HIGH) {
    alarmTriggered = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!!! INTRUDER !!!");
    Serial.println("Motion detected while system is armed");

    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 1000);   // Continuous tone
  }

  // Reset alarm if not triggered
  if (!alarmTriggered) {
    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
  }

  delay(100);  // Small delay for stability
}

