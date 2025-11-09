#include <ArduinoBLE.h>

// === Pin Setup ===
#define LEFT_TRIG 5
#define LEFT_ECHO 7
#define RIGHT_TRIG 6
#define RIGHT_ECHO 8
#define LED 9

// === BLE Service & Characteristic UUIDs ===
#define SERVICE_UUID        "c1d0a000-1234-4abc-bbbb-1234567890ab"
#define DIST_CHAR_UUID      "c1d0a001-1234-4abc-bbbb-1234567890ab"

BLEService visionService(SERVICE_UUID);
BLEStringCharacteristic distChar(DIST_CHAR_UUID, BLERead | BLENotify, 64);

// === Ultrasonic Distance Function ===
float getDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30 ms (~5 m)
  if (duration == 0) return -1; // no echo
  return duration * 0.0343 / 2.0;
}

// === Setup ===
void setup() {
  Serial.begin(9600);
  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);
  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);
  pinMode(LED, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }

  BLE.setLocalName("VisionAssist");
  BLE.setDeviceName("VisionAssist");
  BLE.setAdvertisedService(visionService);

  visionService.addCharacteristic(distChar);
  BLE.addService(visionService);

  distChar.writeValue("{\"status\":\"ready\"}");

  BLE.advertise();
  Serial.println("VisionAssist BLE active. Waiting for connections...");
}

// === Main Loop ===
void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      float left = getDistanceCM(LEFT_TRIG, LEFT_ECHO);
      float right = getDistanceCM(RIGHT_TRIG, RIGHT_ECHO);

      // LED alert if obstacle closer than 15 cm
      if ((left > 0 && left < 15) || (right > 0 && right < 15)) {
        digitalWrite(LED, HIGH);
      } else {
        digitalWrite(LED, LOW);
      }

      // JSON format
      char json[80];
      snprintf(json, sizeof(json), "{\"left\":%.1f,\"right\":%.1f}", left, right);

      distChar.writeValue(json);
      Serial.println(json);

      delay(200);
    }

    Serial.println("Disconnected from central.");
  }
}
