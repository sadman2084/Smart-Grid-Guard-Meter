
#define BLYNK_TEMPLATE_ID "TMPL6VM3nEEY7"
#define BLYNK_TEMPLATE_NAME "Balance"
#define BLYNK_AUTH_TOKEN "1k0Qeqw_VV0UtNbT3fVsiyz0aCfJNnEf"

/*
 * CC-0 by Sharafat Karim and Sadman Kabir
 */
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// #define GREEN_LED 12  // Green LED pin
// #define BLUE_LED 13   // Blue LED pin
// #define RED_LED 14    // Red LED pin
// #define buzzerPin 2

char auth[] = BLYNK_AUTH_TOKEN;

String WIFI_SSID = "American Corner Barishal";  // wifi - username
String WIFI_PASSWORD = "acb01300";              // wifi - password
// String WIFI_SSID = "Sharafat";        // wifi - username
// String WIFI_PASSWORD = "88888888";  // wifi - password

// String WIFI_SSID = "PSTU Teachers";        // wifi - username
// String WIFI_PASSWORD = "PSTU@ICT#2021";  // wifi - password



String HOST_NAME = "https://metro-api-o34i.onrender.com";  // server address
String PATH_NAME = "/users/{id}";                          // server path
String rfidUID;
String balance;
String strVolt;
int balance_value = 0;
String power_mW;

String loadvoltage = "";  // "1"
String current = "";      // "2"
String power = "";
int receivedValue = 0;

BLYNK_WRITE(V5) {
  receivedValue = param.asInt();
  Serial.println(receivedValue);
}

void setup() {
  Serial.begin(9600);


  lcd.init();  // initialize the lcd
  lcd.init();
  lcd.backlight();


  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  connectToWiFi();

  // timer.setInterval(2500L, notifyOnFire);
  // Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD);
  Blynk.begin(auth, WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
}
int electricBill = 0;
void loop() {
  Blynk.run();
  //timer.run();
  //Blynk.config(auth);

  if (Serial.available()) {


    strVolt = Serial.readStringUntil('#');
    strVolt.trim();



    if (strVolt[0] == '*') {
      rfidUID = strVolt;
      Serial.print("Rfid -> ");
      Serial.println(rfidUID);
      rfidUID.replace("*", "");



      Serial.print("you total balance -> ");
      int num = getGetRequest(rfidUID);
      balance_value = num;
      balance = String(num);
      Blynk.virtualWrite(V0, balance_value);

    } else if (rfidUID[0] == '+') {

      Serial.print("Rfid -> ");
      Serial.println(rfidUID);

      int separatorIndex = rfidUID.indexOf(';');
      String val = rfidUID.substring(1, separatorIndex);
      rfidUID = rfidUID.substring(separatorIndex + 1, rfidUID.length());

      // Print the extracted values
      Serial.println("amount = " + val);
      Serial.println("rfidUID = " + rfidUID);

      Serial.print("current -> ");

      int num = getGetRequest(rfidUID);



      Serial.print("updated -> ");
      num = getGetRequest(rfidUID);

      ////////////////////

      ////////////////////
    } else if (strVolt[0] == '$') {
      Serial.print("volt -> ");
      Serial.println(strVolt);
      strVolt.replace("$", "");

      // Find the indices of the semicolons
      int firstSemicolon = strVolt.indexOf(';');
      int secondSemicolon = strVolt.indexOf(';', firstSemicolon + 1);

      // Extract the values
      loadvoltage = strVolt.substring(0, firstSemicolon);                // "1"
      current = strVolt.substring(firstSemicolon + 1, secondSemicolon);  // "2"
      power = strVolt.substring(secondSemicolon + 1);                    // "3"

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Voltage : " + loadvoltage);
      lcd.setCursor(0, 1);
      lcd.print("Current : " + current);
      lcd.setCursor(0, 2);
      lcd.print("Power   : " + power);
      lcd.setCursor(0, 3);
      lcd.print("Balance : " + balance);

      int volt_in_int = loadvoltage.toInt();
      int current_in_int = current.toInt();
      int power_in_int = power.toInt();

      Blynk.virtualWrite(V1, volt_in_int);
      Blynk.virtualWrite(V2, current_in_int);
      Blynk.virtualWrite(V3, power_in_int);

      String strNumber = String(receivedValue);
      sendPUTRequestRecharge(rfidUID, strNumber);


      if ((loadvoltage == "0.00" && current == "0.00" && power == "0.00") || balance == "0.00" || balance == "0") {
        Blynk.virtualWrite(V4, "Meter off");
      } else {
        Blynk.virtualWrite(V4, "Meter on");
      }
      Serial.println("the string is:");
      Serial.println(strVolt);
    }
  }
  // delay(1000);
  for (int i = 0; i < 2; i++) {

    Serial.print("@");
    Serial.print(balance_value);
    Serial.println("#");
    delay(1000);
  }

  if (rfidUID != "" && power != "0.00")
    sendPUTRequestDischarge(rfidUID, "1");

  // delay(1000);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voltage : " + loadvoltage);
  lcd.setCursor(0, 1);
  lcd.print("Current : " + current);
  lcd.setCursor(0, 2);
  lcd.print("Power   : " + power);
  lcd.setCursor(0, 3);
  lcd.print("Balance : " + balance);
}

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.print("Connected!");
}

void dBReference() {
  int returnedValue = getGetRequest("sadman");
  Serial.print("you returned -> ");
  Serial.println(returnedValue);

  sendPUTRequestDischarge("sadman", "1");
}

int getGetRequest(String queryString) {
  HTTPClient http;

  http.begin(HOST_NAME + PATH_NAME + "?key=" + queryString);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);

      http.end();
      return payload.toInt();
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  sleep(1);
  return (0);
}

void sendPUTRequestRecharge(String queryString, String balance) {
  HTTPClient http;

  http.begin(HOST_NAME + "/users/+/{id}?key=" + queryString + "&val=" + balance);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.PUT(queryString);

  // httpCode will be negative on error
  if (httpCode > 0) {
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] PUT... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void sendPUTRequestDischarge(String queryString, String balance) {
  HTTPClient http;

  http.begin(HOST_NAME + "/users/-/{id}?key=" + queryString + "&val=" + balance);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.PUT(queryString);

  // httpCode will be negative on error
  if (httpCode > 0) {
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] PUT... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
