# air-quality-respiratory-health-monitor
🌍 Portable Air Quality & Respiratory Health Monitoring System
What Does This Do?
Imagine a small device you can carry with you that tells you:
Is the air around you clean or polluted? (Detects CO, ammonia, benzene)
What's your heart rate right now? (Real-time monitoring)
Are you getting enough oxygen? (Blood oxygen saturation)
What's the temperature? (Environmental monitoring)
All this data is sent to your phone/laptop via the cloud ☁️ so you can monitor your health anywhere, anytime.
Perfect for: People with respiratory issues, athletes tracking oxygen levels, or anyone monitoring air quality in their workspace.
---
🎯 Why This Project Matters
The Problem:
Air quality varies throughout the day (pollution, traffic)
Respiratory health can change suddenly
Most monitoring devices are expensive or stationary
You can't track your health on-the-go
The Solution:
This portable device monitors your respiratory health AND air quality in real-time, giving you actionable insights about your environment.
Bonus: This project earned a Patent Application (filed jointly with faculty) for its innovative combination of features! 🏆
---
🔧 What's Inside?
Hardware (Components You'll Need)
Component	Model	What It Does
Brain	ESP8266	Processes data & connects to WiFi
Air Quality Sensor	MQ-135	Detects harmful gases (CO, NH3, benzene)
Temperature Sensor	DHT11	Measures temperature & humidity
Pulse Oximeter	MAX30100	Measures heart rate & blood oxygen (SpO2)
Power	USB/Battery	Keeps device running
Why These Sensors?
MQ-135: Industry-standard for air quality detection
DHT11: Cheap, reliable temperature sensor
MAX30100: Clinical-grade pulse oximeter (same tech as fitness watches)
ESP8266: Built-in WiFi + low power = perfect for portable IoT
---
📊 Real-World Performance
After testing (validation reports included), here's what we achieved:
Metric	Result	Target	Status
Cloud Upload Reliability	99.8%	>95%	✅ PASS
Data Loss	<0.2%	<1%	✅ PASS
False Alerts	0 per hour	<0.1 per hour	✅ PASS
Sensor Accuracy	±2%	<5%	✅ PASS
Continuous Runtime	72+ hours	No limit	✅ PASS
---
🏗️ How It's Wired Up
```
ESP8266 Microcontroller (The Brain)
│
├─ D4 ──→ DHT11 (temperature)
├─ D2 (SDA) ──→ MAX30100 I2C (heart rate)
├─ D1 (SCL) ──→ MAX30100 I2C (heart rate)
├─ A0 ──→ MQ-135 (air quality)
│
└─ WiFi (built-in) ──→ ThingSpeak Cloud
                       ↓
                    Your Dashboard
                    (PC/Phone)
```
ThingSpeak Fields:
Field 1: Temperature (°C)
Field 2: Gas Level (0-1023 scale)
Field 3: Heart Rate (bpm)
Field 4: SpO2 Level (%)
---
🚀 Getting Started (5 Minutes)
What You Need
ESP8266 development board
All sensors (listed above)
Arduino IDE installed on your computer
USB cable to program the board
Quick Setup
Step 1: Install Libraries
Open Arduino IDE
Go to: Sketch → Include Library → Manage Libraries
Search and install:
"DHT sensor library" by Adafruit
"MAX30100lib" by OXullo Intersecans
(ESP8266WiFi is built-in)
Step 2: Create ThingSpeak Account
Go to thingspeak.com
Sign up (free account)
Create a new channel
Get your API key
Step 3: Update Your WiFi & API Key
```cpp
const char* ssid = "YOUR_WIFI_NAME";          // Your WiFi network name
const char* password = "YOUR_WIFI_PASSWORD";  // Your WiFi password
String apiKey = "YOUR_THINGSPEAK_API_KEY";    // From ThingSpeak dashboard
```
⚠️ SECURITY WARNING: Don't commit this with your real WiFi password to GitHub! Create a `config.h` file and keep it private.
Step 4: Upload Code
Connect ESP8266 to computer via USB
Select board: Tools → Board → "ESP8266 2.7.4" (or latest)
Select port: Tools → Port → COM3 (or your port)
Click Upload (arrow button)
Step 5: Open Serial Monitor
Tools → Serial Monitor
Set baud rate to 115200
Watch data print in real-time:
```
  Temp: 25.5°C
  Gas Level: 320 ppm
  Heart Rate: 72 bpm
  SpO2: 98%
  ✓ Data sent to ThingSpeak
  ```
---
📱 Viewing Your Data
Once code is running:
Go to your ThingSpeak channel (thingspeak.com)
You'll see 4 live graphs:
Temperature over time
Air quality trends
Heart rate graph
SpO2 levels
Data updates every 15 seconds (customizable)
---
🐛 Issues We Found & Fixed
During development, we discovered and fixed 2 critical bugs. See detailed reports in `Documentation/` folder:
Bug #1: WiFi Uploads Failing
Problem: Device lost 13% of data uploads when WiFi was weak  
Root Cause: Device didn't reconnect automatically if WiFi dropped  
Solution: Added auto-reconnection with exponential backoff  
Result: Improved to 99.8% reliability ✅
Bug #2: False Alerts
Problem: Buzzer activated 8-12 times per hour in clean air  
Root Cause: Sensor noise triggered alerts (noise = ±15 ppm)  
Solution: Added signal averaging + dual-threshold confirmation  
Result: Zero false alerts ✅
This shows: Real-world testing catches issues that simulation misses!
---
🔍 Sensor Calibration (Important!)
Gas sensors aren't 100% accurate out-of-the-box. Here's how to calibrate:
MQ-135 Calibration
Place sensor in clean indoor air (baseline)
Record analog reading: `analogRead(A0)` = ~300
In high-pollution area, record reading = ~500
Map: (0-300) = clean, (300-600) = moderate, (600+) = hazardous
DHT11 Calibration
Test against known reference thermometer
Adjust if needed: `temperature = dht.readTemperature() - 2.0;` (example offset)
MAX30100 Calibration
Sensor auto-calibrates; just ensure proper finger placement
Must be in well-lit area (IR-based sensing)
---
🚨 Troubleshooting
"MAX30100 FAILED"
Check I2C wiring (D2 = SDA, D1 = SCL)
Verify library installed
Try I2C scanner sketch to find device address
"WiFi connection failed"
Check SSID and password are correct
Make sure WiFi is 2.4GHz (ESP8266 doesn't support 5GHz)
Check if WiFi in range
"Data not uploading to ThingSpeak"
Verify API key is correct
Check internet connection
Look at Serial Monitor for error messages
"Sensor readings are NaN (Not a Number)"
DHT11 might be damaged (common issue)
Try replacing with spare sensor
Check pin connections
"Too many false gas alerts"
Sensor needs warmup time (10-20 seconds)
Adjust alert threshold in code: change `500` to higher value
Add sensor calibration
---
📈 Next Steps / Version 2.0 Ideas
This is v1.0! Future improvements could include:
Predictive Analytics
Use machine learning to predict air quality patterns
Notify user before pollution spike
Multi-Device Sync
Connect multiple portable units
Create air quality map of your city
Mobile App
Instead of web dashboard, build native iOS/Android app
Real-time alerts on phone
Local Data Storage
SD card for offline logging
Sync with cloud when WiFi available
Battery Optimization
Deep sleep mode when not in use
Extend battery from hours to weeks
Advanced Filtering
Better noise filtering for sensors
Machine learning noise detection
Wearable Form Factor
Miniaturize into wristband or pendant
Combine with smartwatch
---
📚 Understanding the Code
Main Loop Breakdown
```cpp
void loop() {
  // 1. Read all sensors
  pox.update();                    // Update pulse oximeter
  heartRate = pox.getHeartRate();  // Get heart rate
  spo2 = pox.getSpO2();            // Get blood oxygen
  
  // 2. Read environmental sensors
  float temperature = dht.readTemperature();  // Get temperature
  int gasValue = analogRead(MQ135);           // Get gas level
  
  // 3. Print to Serial (for debugging)
  Serial.print("Temp: ");
  Serial.println(temperature);
  
  // 4. Upload to cloud
  if (client.connect(server, 80)) {
    // Create URL with all sensor readings
    String url = "/update?api_key=" + apiKey + 
                 "&field1=" + String(temperature) + 
                 "&field2=" + String(gasValue) + 
                 "&field3=" + String(heartRate) + 
                 "&field4=" + String(spo2);
    
    // Send to ThingSpeak
    client.print(...http request...);
  }
  
  // 5. Wait 15 seconds before next reading
  delay(15000);
}
```
Key Concept: Every 15 seconds, device:
Reads all sensors
Prints to Serial (for debugging)
Sends to ThingSpeak
Waits for next cycle
---
🔐 Security Notes
⚠️ IMPORTANT: Don't Commit Secrets to GitHub!
Your code has hardcoded WiFi & API key visible to everyone:
```cpp
const char* ssid = "LUX";                    // 🚨 Anyone can see this!
const char* password = "Luxdv@07";           // 🚨 Anyone can see this!
String apiKey = "9SR2FRGUGL1FSKKX";         // 🚨 Anyone can see this!
```
Solution: Create `secrets.h` (not committed to GitHub)
```cpp
// secrets.h (ADD TO .gitignore)
#ifndef SECRETS_H
#define SECRETS_H

const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";
const char* THINGSPEAK_API = "YOUR_API_KEY";

#endif
```
Then in main code:
```cpp
#include "secrets.h"  // Won't be on GitHub!
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
String apiKey = THINGSPEAK_API;
```
---
📄 File Structure
```
air-quality-respiratory-health-monitor/
├── README.md (this file)
├── LICENSE (MIT)
├── .gitignore (excludes secrets.h)
│
├── Code/
│   ├── air_quality_monitor.ino (main sketch)
│   └── secrets.h (create this locally - NOT on GitHub)
│
├── Documentation/
│   ├── VALIDATION_REPORT.md (testing results)
│   ├── BUG_REPORT.md (issues found & fixed)
│   ├── API_DOCUMENTATION.md (ThingSpeak fields)
│   └── TROUBLESHOOTING.md (common issues)
│
├── Hardware/
│   ├── pin_connections.txt
│   ├── components_list.txt
│   └── circuit_diagram.png
│
└── Tests/
    ├── sensor_calibration_log.txt
    └── performance_test_results.txt
```
---
🎓 What This Project Teaches
This isn't just a hobby project—it covers real engineering:
✅ Embedded Systems: Microcontroller programming  
✅ Sensor Integration: Multiple sensor types + calibration  
✅ Wireless Communication: WiFi & cloud connectivity  
✅ Signal Processing: Noise filtering + data averaging  
✅ Cloud Architecture: Sending data to remote servers  
✅ Testing & Validation: Systematic testing methodology  
✅ Debugging: Finding & fixing real-world issues  
✅ Documentation: Professional report writing  
✅ Product Design: End-to-end system thinking
---
👤 About Me
Latchana Devi M V
B.Tech, Electronics & Communication Engineering (Batch 2027)
CGPA: 9.16/10
SRM Institute of Science and Technology, Thiruvarur, TN
📧 Email: moorthittp@gmail.com  
🔗 LinkedIn: linkedin.com/in/latchana-devi-7484a6358  
💻 GitHub: github.com/Latchana-Devi
---
📜 Patent Status
Patent Application Filed ✅  
Title: "Portable Air Quality and Respiratory Health Monitoring System"  
Co-filed with Faculty Guide (Pending Approval)
This shows the project's innovation value beyond typical hobby projects.
---
📝 License
MIT License - Free to use, modify, and distribute (see LICENSE file for details)
---
🙏 Thanks
Faculty Guide (Patent co-filer)
Adafruit (DHT library)
OXullo Intersecans (MAX30100 library)
ThingSpeak (free IoT cloud platform)
SRM Institute of Science and Technology
---
📞 Questions?
Have questions about the project?
Open a GitHub Issue
Email: moorthittp@gmail.com
Check TROUBLESHOOTING.md first!
---
