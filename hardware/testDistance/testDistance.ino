#include <ArduinoBLE.h>

// === Pin Setup ===
#define LEFT_TRIG 5
#define LEFT_ECHO 7
#define RIGHT_TRIG 6
#define RIGHT_ECHO 8
#define LED 9

// BLE UUIDs
#define SERVICE_UUID "c1d0a000-1234-4abc-bbbb-1234567890ab"
#define CHAR_UUID    "c1d0a001-1234-4abc-bbbb-1234567890ab"

// BLE Objects
BLEService visionService(SERVICE_UUID);
BLEStringCharacteristic distChar(CHAR_UUID, BLERead | BLENotify, 64);

// === Non-blocking distance function ===
float getDistanceCM(int trigPin, int echoPin) {
  // Trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Wait for echo start (LOW → HIGH)
  unsigned long startTime = micros();
  while (digitalRead(echoPin) == LOW) {
    BLE.poll();  // service BLE stack
    if (micros() - startTime > 30000) return -1; // timeout 30 ms
  }

  // Measure HIGH duration
  unsigned long echoStart = micros();
  while (digitalRead(echoPin) == HIGH) {
    BLE.poll();  // service BLE stack
    if (micros() - echoStart > 30000) return -1; // timeout 30 ms
  }

  unsigned long duration = micros() - echoStart;
  return duration * 0.0343 / 2.0;
}

// === Setup ===
void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);
  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);
  pinMode(LED, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("BLE failed to start");
    while (1);
  }

  BLE.setLocalName("VisionAssist");
  BLE.setDeviceName("VisionAssist");
  BLE.setAdvertisedService(visionService);

  visionService.addCharacteristic(distChar);
  BLE.addService(visionService);

  distChar.writeValue("{\"status\":\"ready\"}");
  BLE.advertise();

  Serial.println("Advertising as VisionAssist...");
}

// === Loop ===
void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to: ");
    Serial.println(central.address());

    while (central.connected()) {
      float left = getDistanceCM(LEFT_TRIG, LEFT_ECHO);
      float right = getDistanceCM(RIGHT_TRIG, RIGHT_ECHO);

      // LED on if object <15 cm on either side
      digitalWrite(LED, (left > 0 && left < 15) || (right > 0 && right < 15));

      char jsonBuffer[80];
      snprintf(jsonBuffer, sizeof(jsonBuffer),
               "{\"left\":%.1f,\"right\":%.1f}", left, right);

      distChar.writeValue(jsonBuffer);
      Serial.println(jsonBuffer);

      BLE.poll();
      delay(200); // allow BLE to handle notifications
    }

    Serial.println("Disconnected");
    BLE.advertise();
  }

  BLE.poll();
}
