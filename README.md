# Smart Grid Guard Meter  

The **Smart Grid Guard Meter** is a prepaid electricity meter system with enhanced security and monitoring capabilities. It integrates RFID-based recharging, real-time power metrics, and theft detection to ensure efficient and secure energy usage. The system also connects to the cloud for data storage and uses the Blynk IoT app for mobile monitoring and management.  

---

## Features  

- **Prepaid Meter System**: Recharge via RFID-based vending machine, with balance stored on the RFID card.  
- **Cloud Integration**: Transaction and power data are stored securely using FASTAPI.  
- **Power Monitoring**: Calculates and displays current, voltage, power, and balance.  
- **Anti-Theft Mechanisms**:  
  - Detects unauthorized tampering using a custom touch sensor.  
  - Limits power usage to identify and shut down theft attempts.  
- **IoT Monitoring and Control**: Provides real-time monitoring via the Blynk mobile app, with the option to update the balance directly from the app.  
- **Audio Feedback**: Uses a DFPlayer module to provide alerts.  
- **Real-Time Display**: Displays power metrics on an I2C LCD connected to ESP32.  

---

## Code Overview  

### 1. **Administration Part**  
- Allows recharging the meter using an RFID vending machine.  
- Stores the balance directly on the RFID card.  
- Uploads recharge data to the cloud via FASTAPI.  

### 2. **Customer Part (Arduino 1)**  
- Scans the RFID card for balance information.  
- Sends balance data to Arduino 2 (`uno_prepaid_meter` code) for further processing.  

### 3. **Uno Prepaid Meter Code (Arduino 2)**  
- **Core Functionality**:  
  - Calculates **current**, **voltage**, **power**, and **remaining balance**.  
  - Detects tampering using a **touch sensor** built with a transistor.  
  - Implements a **power limitation system** to identify theft attempts.  
  - Sends calculated data to ESP32 for further processing.  
- **Audio Feedback**: Plays alerts using a DFPlayer module.  

### Why Use a Touch Sensor?  
The touch sensor detects if any metal or cable is used to steal electricity from the meter, providing an additional layer of security.  

### Why Use Power Limitation?  
Limiting power helps detect theft attempts where unauthorized usage increases power consumption. If theft is detected:  
1. The system shuts down automatically.  
2. Reactivation requires the RFID card.  

### 4. **ESP32 Code**  
- Receives data from Arduino 2.  
- Displays **current**, **voltage**, **power**, and **balance** on an **I2C LCD**.  
- Uploads all metrics to the cloud.  
- Sends real-time data to the Blynk IoT app for mobile monitoring.  

---

## IoT Integration with Blynk  

The system uses the **Blynk IoT app** for seamless mobile monitoring and management:  

- **Real-Time Monitoring**: View live metrics, including current, voltage, power, and balance, directly from your smartphone.  
- **Balance Input**: Add balance directly from the mobile app. This balance will be updated in the database and synchronized with the system.  
- **Alerts**: Receive notifications for low balance, theft detection, and other system events.  

---

## Hardware Requirements  

- **Arduino Uno (x2)**: For RFID processing and power calculation.  
- **ESP32**: For cloud integration and IoT monitoring.  
- **RFID Reader and Cards**: For prepaid recharges.  
- **DFPlayer Module**: Provides audio alerts.  
- **Touch Sensor**: Built using a transistor to detect tampering.  
- **I2C LCD**: Displays real-time metrics.  
- **Power Monitoring Sensors**: For current, voltage, and power measurements.  

---

## Installation  

1. **Set Up Hardware**: Connect all components as per the project schematic.  
2. **Upload Code**: Upload the respective code to **Arduino 1**, **Arduino 2**, and **ESP32**.  
3. **Cloud and IoT Setup**:  
   - Configure FASTAPI and database for backend.  
   - Set up the Blynk IoT app for monitoring and balance management.  
4. **Run the System**:  
   - Use RFID cards or the Blynk app to recharge.  
   - Monitor power usage and theft detection via the Blynk app.  

---
