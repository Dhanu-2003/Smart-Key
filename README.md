# **Wireless Smart Bike Key System**  
This project integrates IoT technology to create a secure, feature-rich bike key system. It utilizes ESP8266 modules for communication, a GPS module for tracking, and a Flask-based backend for data management.  

---

## **Features**
- **Wireless Key Authentication**: Start the bike using an ESP8266-based smart key.  
- **GPS Tracking**: Track the bike's location, speed, and distance traveled.  
- **Time-Controlled Duplicate Keys**: Assign specific time slots for duplicate keys to work.  
- **Remote Monitoring**: View bike usage statistics and GPS data via the backend.  
- **Anti-Theft Measures**: Secure bike access to prevent unauthorized use.  

---

## **Hardware Requirements**
- ESP8266 Modules (x3)
- GPS Module (e.g., NEO-6M)
- Relay Module
- DC Motor (for demonstration)
- Power Supply (5V or appropriate)
  
---

## **Software Requirements**
- Python 3.x
- Flask Framework
- Arduino IDE
- ESP-NOW library for ESP8266  

---

## **Setup Instructions**

### **1. Hardware Setup**  
1. Connect the ESP8266 modules as follows:  
   - One module in the smart key.  
   - One module in the bike receiver.  
   - One module connected to the GPS module.  
2. Attach the relay to the ESP8266 in the bike receiver for bike ignition simulation.  
3. Connect the DC motor to the relay for demonstration purposes.  

---

### **2. Software Setup**

#### **2.1. Backend Setup**
1. Clone the repository:  
   ```bash
   git clone <repository-link>
   cd WirelessSmartBikeKey
   ```  
2. Install dependencies:  
   ```bash
   pip install -r requirements.txt
   ```  
3. Start the server:  
   ```bash
   python app.py
   ```  
4. Copy the IP address displayed in the terminal (e.g., `http://192.168.x.x:5000`).  

---

#### **2.2. ESP8266 Firmware Setup**  
1. Open the **Arduino IDE**.  
2. Install the ESP8266 board support:  
   - Go to *File > Preferences > Additional Board Manager URLs* and add:  
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```  
   - Install the **ESP8266** package from *Tools > Board > Boards Manager*.  
3. Install required libraries:  
   - ESP-NOW  
   - SoftwareSerial (for GPS communication)  
4. Update the code:  
   - Open the **GPS_DATA_TRANSFER.ino** file.  
   - Replace the `BACKEND_IP` placeholder with the IP address copied from the terminal.  
5. Upload the respective code files to the ESP8266 modules:  
   - **KEY_MODULE.ino**: For the smart key.  
   - **BIKE_MODULE.ino**: For the bike receiver.  
   - **GPS_DATA_TRANSFER.ino**: For GPS data transmission.  

---

### **3. Deployment and Usage**

#### **3.1. Network Configuration**
1. Connect all ESP8266 modules and your laptop to the same Wi-Fi hotspot.  

#### **3.2. GPS Initialization**
1. Place the GPS module in an open space with a clear sky view.  
2. Wait a few minutes for the GPS module to acquire a signal.  

#### **3.3. Key Operation**
1. Power on the key module and bike receiver.  
2. Press the key button to transmit data.  
3. If the data is valid, the bike will start, and the motor will activate.  

#### **3.4. Monitor Backend**
1. Access the backend dashboard using the IP address in a web browser.  
2. View GPS data, bike status, and key usage statistics.  

---

## **Troubleshooting**
1. **GPS Not Connecting**: Ensure the GPS module is in an open area with a clear sky.  
2. **Bike Not Starting**: Check the ESP-NOW pairing and authentication logic.  
3. **Backend Not Loading**: Ensure Flask is installed and running. Check the Wi-Fi connection.  

---

## **Future Improvements**
- Develop a mobile application for enhanced user experience.  
- Integrate geofencing for location-based access control.  
- Implement advanced encryption for secure communication.  

---
