#include <Wire.h>
#include <Adafruit_INA219.h>
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

#define SLAVE_ADDRESS 8  // I2C address for communication with Arduino 1
Adafruit_INA219 ina219;


String receivedData = "";

SoftwareSerial mySoftwareSerial(10, 11);
DFRobotDFPlayerMini myDFplayer;

int relaypin = 7;  // Pin to control the relay

// Authorized UID
String authorizedUID1 = "43422f1c";
String authorizedUID2 = "826b61e4";
String authorizedUID3 = "5391fda";
bool meterStopped = true;
String uid = "";
String balance = "";
String balanceString = "0";

int electricBill = 0;
bool firstRecharge = false;
bool validUIDReceived = false;
int analogValue;

void setup() {
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);

  Wire.begin(SLAVE_ADDRESS);   // Initialize I2C communication as a slave with address 8
  Wire.onReceive(receiveUID);  // Set the function to be called when data is received

  // Initialize INA219 sensor
  if (!ina219.begin()) {
    Serial.println("INA219 initialization failed");
    while (1)
      ;
  }

  // Initialize relay pin
  pinMode(relaypin, OUTPUT);
  digitalWrite(relaypin, HIGH);  // Ensure relay is off initially


  if (!myDFplayer.begin(mySoftwareSerial)) {
    Serial.println("DFPlayer Mini not detected!");
    while (true)
      ;  // Halt if initialization fails
  }

  myDFplayer.volume(80);  // Set volume
  Serial.println("DFPlayer Mini initialized.");
}

void loop() {



  // If a valid UID is received, show INA219 data
  if (validUIDReceived) {
    displayINA219Data();  // Display INA219 data
    //task1();
    // delay(2000);          // Delay 2 seconds before displaying again

    if (!firstRecharge) {
      balanceRelodedVoice();
      firstRecharge = true;
    }
  }




  //////////////////
  if (Serial.available()) {  // Check if there is data available to read
    String receivedData = Serial.readStringUntil('#');
    receivedData.trim();

    if (receivedData[0] == '@') {
      balance = receivedData;
      balance.replace("@", "");
      Serial.println("Received from ESP32: " + balance);  // Print the received string to Serial Monitor
    }
    // String receivedData = Serial.readString();               // Read the string from ESP32
  }
  /////////////////////////////////
}

// void task1() {
//   analogValue = analogRead(A0);
//   Serial.println(analogValue);
// }

// Function to receive UID from Arduino 1
void receiveUID(int byteCount) {
  uid = "";  // Clear previous UID
  while (Wire.available()) {
    char c = Wire.read();  // Read each byte sent
    uid += c;
  }

  uid.toLowerCase();  // Convert to lowercase for consistency
  Serial.print("Received UID: ");
  Serial.println(uid);

  // Print UID 2 times with * and # for demonstration
  for (int i = 0; i < 2; i++) {
    Serial.print("*");
    Serial.print(uid);
    Serial.println("#");
    delay(1000);
  }

  // If a valid UID is received, set the flag to display INA219 data
  if (uid == authorizedUID1 || uid == authorizedUID2 || uid == authorizedUID3) {
    validUIDReceived = true;      // Set flag to true to start displaying data
    digitalWrite(relaypin, LOW);  // Turn on relay when a valid UID is scanned
  } else {
    validUIDReceived = false;      // Invalid UID, stop displaying data and turn off relay
    digitalWrite(relaypin, HIGH);  // Turn off relay for invalid UID
  }
}

// Function to collect and display INA219 data
void displayINA219Data() {
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  // Read INA219 data
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  // Display the INA219 values on the Serial Monitor
  Serial.println("INA219 Sensor Data:");

  Serial.print("Bus Voltage:   ");
  Serial.print(busvoltage);
  Serial.println(" V");

  Serial.print("Shunt Voltage: ");
  Serial.print(shuntvoltage);
  Serial.println(" mV");

  Serial.print("Load Voltage:  ");
  Serial.print(loadvoltage);
  Serial.println(" V");

  Serial.print("Current:       ");
  Serial.print(current_mA);
  Serial.println(" mA");

  Serial.print("Power:         ");
  Serial.print(power_mW);
  Serial.println(" mW");
  //Serial.println("the fucking balance is:" + receivedData);
  Serial.println("");  // Add a line break




  for (int i = 0; i < 5; i++) {
    analogValue = analogRead(A0);
    Serial.println(analogValue);

    if (analogValue < 500) {
      Serial.println("Touch something, turning off relay.");
      digitalWrite(relaypin, HIGH);

      validUIDReceived = false;

      contactAdminVoice();
    }
  }
  //////////////////////sending to esp32
  String output = String(loadvoltage) + ";" + String(current_mA) + ";" + String(power_mW);
  int balanceInt = balance.toInt();
  Serial.println(output);

  /////////////////sending volt
  // String strVolt = String(busvoltage);
  for (int i = 0; i < 2; i++) {

    Serial.print("$");
    Serial.print(output);
    Serial.println("#");
    delay(1000);
  }
  ///////

  /////////////////sending UID
  for (int i = 0; i < 2; i++) {
    Serial.print("*");
    Serial.print(uid);
    Serial.println("#");
    delay(1000);
  }


  if (power_mW > 70) {
    Serial.println("Power exceeded 70mW, turning off relay.");
    digitalWrite(relaypin, HIGH);

    validUIDReceived = false;

    contactAdminVoice();
  } else if (balance == "0" || balance == "0.00") {
    Serial.println("Balance is 0 now, turning off relay.");
    digitalWrite(relaypin, HIGH);

    // validUIDReceived = false;

    notEnoughBalanceVoice();
  }
}




void contactAdminVoice() {
  myDFplayer.playFolder(3, 1);  // Play file 1 from folder 3
  delay(10000);                 // Add delay to avoid rapid playback
}


void notEnoughBalanceVoice() {
  myDFplayer.playFolder(1, 1);  // Play file 1 from folder 3
  delay(5000);                  // Add delay to avoid rapid playback
}

void balanceRelodedVoice() {
  myDFplayer.playFolder(2, 1);  // Play file 1 from folder 3
  delay(5000);                  // Add delay to avoid rapid playback
}