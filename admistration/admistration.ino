#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <MyUltrasonicLibrary.h>
#include <SPI.h>
#include <Wire.h>
MyUltrasonicLibrary ultrasonic(5, 8);
#define RST_PIN 9
#define SS_PIN 10
const int brightnessPin1 = 3;
const int brightnessPin2 = 6;
const int LDRPin = A5;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = { { '1', '2', '3', 'A' },
                          { '4', '5', '6', 'B' },
                          { '7', '8', '9', 'C' },
                          { '*', '0', '#', 'D' } };

byte rowPins[ROWS] = { A0, A1, A2, A3 };
byte colPins[COLS] = { 7, 4, 2, 5 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int balance = 300;
String uidString = "";

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  lcd.init();  // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();

  pinMode(brightnessPin1, OUTPUT);
  pinMode(brightnessPin2, OUTPUT);

  while (!Serial)
    ;
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);

  int LDRValue = analogRead(LDRPin);

  if (LDRValue >= 600) {

    analogWrite(brightnessPin1, 255);
    analogWrite(brightnessPin2, 255);
  } else {

    analogWrite(brightnessPin1, 128);
    analogWrite(brightnessPin2, 128);
  }

  // Optional delay
}

void loop() {

  unsigned long startTime = millis();
  bool cardScanned = false;


  while (millis() - startTime < 10000) {
    if (forRFID()) {
      cardScanned = true;
      showMainMenu();
      break;
    }
  }


  if (!cardScanned) {
    lcd.setCursor(0, 0);
    lcd.print("Please scan your");
    lcd.setCursor(0, 1);
    lcd.print("card.");
    delay(2000);
  }
}


bool forRFID() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }

  Serial.print(F("UID: "));
  Serial.println(uidString);



  mfrc522.PICC_HaltA();
  return true;
}

void showMainMenu() {


  int distance = ultrasonic.getDistance();

  int LDRValue = analogRead(LDRPin);

  if (LDRValue >= 600) {

    analogWrite(brightnessPin1, 255);
    analogWrite(brightnessPin2, 255);
  } else {

    analogWrite(brightnessPin1, 128);
    analogWrite(brightnessPin2, 128);
  }


  if (distance < 150) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Choose an option:");
    delay(2000);
    lcd.clear();
  }
  lcd.setCursor(0, 0);
  lcd.print("1. Check Balance");
  lcd.setCursor(0, 1);
  lcd.print("2. Add Balance");


  char keyp = keypad.getKey();
  while (keyp == NO_KEY) {
    keyp = keypad.getKey();
  }
  lcd.clear();
  Serial.print("Key Pressed: ");
  Serial.println(keyp);

  int option1 = keyp - '0';

  if (option1 == 1) {
    checkBalance();

  } else if (option1 == 2) {
    addBalanceMenu();

  } else {
    lcd.setCursor(0, 0);
    lcd.print("Invalid option.");
    delay(2000);
    lcd.clear();

    showMainMenu();
  }
}

void checkBalance() {


  lcd.setCursor(0, 0);
  lcd.print("Checking balance..");
  delay(2000);
  lcd.clear();


  for (int i = 0; i < 2; i++) {

    Serial.print("*");
    Serial.print(uidString);
    Serial.println("#");
    delay(1000);
  }

  delay(2000);
  showMainMenu();
}

void addBalanceOptions() {


  lcd.setCursor(0, 0);
  lcd.print("Add new balance");
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("1.Enter amount");

  lcd.setCursor(0, 1);
  lcd.print("2.Main menu");

  char keyp = keypad.getKey();
  while (keyp == NO_KEY) {
    keyp = keypad.getKey();
  }
  delay(1000);
  lcd.clear();

  Serial.print("Key Pressed: ");
  Serial.println(keyp);

  int option2 = keyp - '0';

  if (option2 == 1) {

    lcd.setCursor(0, 0);
    lcd.print("Enter amount to");
    lcd.setCursor(0, 1);
    lcd.print("add:");


    String inputString = "";
    char key;

    key = keypad.getKey();

    do {
      key = keypad.getKey();
      if (key && key != '*') {
        inputString += key;
      }
      Serial.println("->" + inputString + "-< ");
    } while (key != '*');

    Serial.println(inputString);
    lcd.println(inputString);

    for (int i = 0; i < 2; i++) {

      Serial.print("+" + inputString + ";");
      Serial.print(uidString);
      Serial.println("#");
      delay(1000);
    }


    showMainMenu();
  } else if (option2 == 2) {
    showMainMenu();
  } else {
    Serial.println("Invalid option.");
  }
}

void addBalanceMenu() {


  lcd.setCursor(0, 0);
  lcd.print("How do you like");
  lcd.setCursor(0, 1);
  lcd.print("to pay?");
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("1.Using Machine");
  lcd.setCursor(0, 1);
  lcd.print("2.Using Online");

  char keyp = keypad.getKey();
  while (keyp == NO_KEY) {
    keyp = keypad.getKey();
  }
  Serial.print("Key Pressed: ");
  Serial.println(keyp);

  lcd.clear();

  int option3 = keyp - '0';

  if (option3 == 1) {


    lcd.setCursor(0, 1);
    lcd.print("Paying using Machine...");
    delay(3000);
    lcd.clear();

  } else if (option3 == 2) {


    lcd.setCursor(0, 0);
    lcd.print("Paying Online...");
    delay(3000);
    lcd.clear();

    addBalanceOptions();

  } else {


    lcd.setCursor(0, 1);
    lcd.print("Invalid option.");
    delay(3000);
    lcd.clear();
  }

  while (Serial.available() > 0) {
    Serial.read();
  }

  delay(1000);
}