# Troubleshooting Guide - Air Quality Monitor

## Problem: "MAX30100 FAILED" Error

**What it means:** Pulse oximeter sensor not responding

**How to fix:**
1. Check I2C wiring:
   - D2 should connect to SDA
   - D1 should connect to SCL
2. Verify library installed: Search "MAX30100lib" in Arduino IDE → Install
3. Check if sensor is powered (5V)
4. Try I2C scanner sketch to verify device communication

---

## Problem: "WiFi connection failed"

**What it means:** Device cannot connect to WiFi network

**How to fix:**
1. Check SSID (WiFi name) and password in `secrets.h`
2. Verify WiFi is 2.4GHz (ESP8266 doesn't support 5GHz)
   - Go to your router settings
   - Check if 2.4GHz band is enabled
3. Check if you're in WiFi range
4. Restart your router
5. Restart the ESP8266 (unplug/replug USB)

---

## Problem: "Data not uploading to ThingSpeak"

**What it means:** WiFi connected but data not reaching cloud

**How to fix:**
1. Verify API key is correct in `secrets.h`
2. Check internet connection (try laptop/phone)
3. Open Serial Monitor (Tools → Serial Monitor)
4. Set baud rate to 115200
5. Watch for error messages
6. If timeout error: Check firewall settings
7. If connection refused: ThingSpeak server might be down (check thingspeak.com)

---

## Problem: "Sensor readings are NaN (Not a Number)"

**What it means:** DHT11 sensor not responding or broken

**How to fix:**
1. Check DHT11 wiring:
   - D4 (data pin) is connected
   - Power (5V) and GND are connected
2. DHT11 sensors fail easily—try replacing with new sensor
3. If soldered, check for cold solder joints
4. Ensure sensor is not near heat source
5. Wait 2 seconds after power-on before reading (sensor needs warmup)

---

## Problem: "Too many false gas alerts"

**What it means:** Buzzer activating when air quality is actually clean

**How to fix:**
1. Wait 20-30 seconds for sensor warmup after power-on
2. Increase alert threshold in code:
   - Find: `if (smoothedGasValue > 500)`
   - Change 500 to 600 or 700 (higher = less sensitive)
3. Ensure sensor is not near smoke, perfume, or strong odors during calibration
4. Perform sensor calibration:
   - Place in clean air for 2 minutes
   - Note the "baseline" reading
   - Set alert threshold 20-30% higher

---

## Problem: Device doesn't turn on / No Serial output

**What it means:** Device not powered or not communicating

**How to fix:**
1. Check USB cable is properly connected to ESP8266
2. Check if USB port is powered (try different port on computer)
3. In Arduino IDE:
   - Go to Tools → Board → Select "ESP8266 2.7.4"
   - Go to Tools → Port → Select correct COM port
   - If no COM port appears: Install CP2102 USB driver
4. Click Upload button and watch for:
   - `Connecting...` messages
   - Progress bar filling up
   - `Done uploading` message

---

## Problem: "Device works fine but no WiFi LED indicator"

**What it means:** WiFi works but status LED not lighting up

**How to fix:**
1. Check LED is in correct pin (D0)
2. Check LED polarity (longer leg = positive)
3. In code, verify: `#define STATUS_LED D0`
4. Try this test sketch:
```cpp
   void setup() {
     pinMode(D0, OUTPUT);
   }
   void loop() {
     digitalWrite(D0, HIGH);
     delay(500);
     digitalWrite(D0, LOW);
     delay(500);
   }
```
5. If LED blinks, your LED works and issue is in main code
6. If LED doesn't blink, LED or resistor may be broken

---

## Problem: "Serial Monitor shows garbage characters"

**What it means:** Baud rate mismatch between code and monitor

**How to fix:**
1. In Serial Monitor, look at bottom right
2. Change baud rate to **115200**
3. Should now show clear messages like "Initializing Air Quality Monitor..."

---

## Problem: "Sensor reading spikes randomly"

**What it means:** Electrical noise interfering with analog readings

**How to fix:**
1. Add capacitor (0.1µF) across sensor power and ground
2. Keep wires short (especially A0 to MQ135)
3. Keep away from WiFi router and other RF sources
4. Code already has signal averaging—spikes should be rare
5. If still happening, check power supply stability (should be 5V ±0.2V)

---

## Problem: "ThingSpeak shows old data"

**What it means:** Device might have stopped uploading

**How to fix:**
1. Check if device is still powered
2. Check Serial Monitor for error messages
3. Reset the device (unplug/replug USB)
4. Check if WiFi is still connected
5. If WiFi works but upload fails, check:
   - Internet connection on computer/phone
   - ThingSpeak website status (thingspeak.com)
   - API key in `secrets.h` is correct

---

## Quick Checklist

Before assuming device is broken:

- [ ] All sensors connected properly?
- [ ] Power supply connected (5V)?
- [ ] USB cable working (try different cable)?
- [ ] Arduino IDE has correct board selected (ESP8266)?
- [ ] Correct COM port selected?
- [ ] Baud rate in Serial Monitor = 115200?
- [ ] All libraries installed?
- [ ] WiFi SSID and password correct?
- [ ] ThingSpeak API key correct?
- [ ] Device warmed up (wait 30 seconds after power-on)?

---

## Still Not Working?

1. Take a photo of your wiring setup
2. Check GitHub Issues or create new one
3. Include:
   - Serial Monitor output (screenshot)
   - Wiring diagram
   - Exact error message
   - What you've already tried
