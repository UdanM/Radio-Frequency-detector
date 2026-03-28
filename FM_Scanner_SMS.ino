#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial sim900(7, 8); // RX, TX

#define TEA5767_ADDR 0x60
#define PHONE_NUMBER "+1234567890" // number
#define SCAN_START   875          // 87.5 MHz * 10
#define SCAN_END     1080         // 108.0 MHz * 10
#define SCAN_STEP    2            // 0.2 MHz steps
#define SIGNAL_THRESHOLD 35

bool setFrequency(float freq) {
  uint32_t freqWord = (uint32_t)((freq * 1000000 + 225000) / 8192);
  uint8_t buf[5];
  buf[0] = (freqWord >> 8) & 0x3F;
  buf[1] = freqWord & 0xFF;
  buf[2] = 0x90;
  buf[3] = 0x1E;
  buf[4] = 0x00;
  Wire.beginTransmission(TEA5767_ADDR);
  Wire.write(buf, 5);
  return Wire.endTransmission() == 0;
}

uint8_t getSignalLevel() {
  Wire.requestFrom(TEA5767_ADDR, 5);
  uint8_t data[5] = {0};
  for (int i = 0; i < 5 && Wire.available(); i++) data[i] = Wire.read();
  return (data[3] >> 4) & 0x0F;
}

void simSend(const char* cmd, uint16_t wait = 1000) {
  sim900.println(cmd);
  delay(wait);
}

bool initSIM900() {
  simSend("AT");
  simSend("AT+CMGF=1");
  return true;
}

void sendSMS(const String& msg) {
  sim900.print("AT+CMGS=\"");
  sim900.print(PHONE_NUMBER);
  sim900.println("\"");
  delay(500);
  sim900.print(msg);
  sim900.write(26); 
  delay(5000);
}

void setup() {
  Serial.begin(9600);
  sim900.begin(9600);
  Wire.begin();
  delay(2000);
  initSIM900();
  Serial.println("FM Scanner ready.");
}

void loop() {
  Serial.println("Scanning...");
  String found = "Available FM stations:\n";
  int count = 0;

  for (int f = SCAN_START; f <= SCAN_END; f += SCAN_STEP) {
    float freq = f / 10.0;
    setFrequency(freq);
    delay(60);
    uint8_t level = getSignalLevel();
    if (level >= SIGNAL_THRESHOLD) {
      Serial.print("Found: ");
      Serial.print(freq);
      Serial.print(" MHz  Signal: ");
      Serial.println(level);
      found += String(freq, 1) + " MHz\n";
      count++;
    }
  }

  if (count == 0) found += "No stations detected.";

  Serial.println("Scan complete. Sending SMS...");
  sendSMS(found);
  Serial.println("SMS sent.");

  delay(300000); // Repeat every 5 minutes
}
