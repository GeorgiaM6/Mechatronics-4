#include <Servo.h>
#include <LiquidCrystal_I2C.h>

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int SERVO_PIN = 6;
const int POT_PIN = A0;

Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

long duration;
int distanceCm;
int potValue;
int detectionRange; 
bool carDetected = false; 
bool lastCarDetected = false; 
int lastDisplayedRange = 0; 

void setup() {
  Serial.begin(9600);
  Serial.println("Car Park System Starting...");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  delay(500);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Car Park System");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  
  lcd.clear();
  updateDisplay(false, detectionRange);
  
  Serial.println("Setup Complete!");
}

void loop() {
  potValue = analogRead(POT_PIN);
  detectionRange = map(potValue, 0, 1023, 10, 335);

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  duration = pulseIn(ECHO_PIN, HIGH);
  
  if (duration == 0) {
    distanceCm = 999;
  } else {
    distanceCm = duration * 0.0343 / 2;
  }

  if (distanceCm > 0 && distanceCm < detectionRange) {
    carDetected = true;
    myServo.write(90); 
    Serial.print("CAR DETECTED at ");
    Serial.print(distanceCm);
    Serial.print("cm (Range: ");
    Serial.print(detectionRange);
    Serial.println("cm) - Barrier UP");
  } else {
    carDetected = false;
    myServo.write(0); 
    Serial.print("NO CAR - Distance: ");
    Serial.print(distanceCm);
    Serial.print("cm (Range: ");
    Serial.print(detectionRange);
    Serial.println("cm) - Barrier DOWN");
  }

  if (carDetected != lastCarDetected || detectionRange != lastDisplayedRange) {
    updateDisplay(carDetected, detectionRange);
    lastCarDetected = carDetected;
    lastDisplayedRange = detectionRange;
  }

  delay(100); 
}

void updateDisplay(bool carPresent, int range) {
  lcd.clear();
  
  lcd.setCursor(0, 0);
  if (carPresent) {
    lcd.print("CAR DETECTED!   ");
  } else {
    lcd.print("NO CAR          ");
  }
  
  lcd.setCursor(0, 1);
  if (carPresent) {
    lcd.print("UP Range: ");
  } else {
    lcd.print("DOWN Range: ");
  }
  
  lcd.print(range);
  lcd.print(" cm");
}