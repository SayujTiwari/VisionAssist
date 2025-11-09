#include <ArduinoBLE.h>

// === Pin Setup ===
#define LEFT_TRIG 5
#define LEFT_ECHO 7
#define RIGHT_TRIG 6
#define RIGHT_ECHO 8
#define LED 9

// BLE UUIDs
#define SERVICE_UUID   "c1d0a000-1234-4abc-bbbb-1234567890ab"
#define CHAR_UUID      "c1d0a001-1234-4abc-bbbb-1234567890ab"

// LE Objects
BLEService visionService(SERVICE_UUID);
BLEStringCharacteristic distChar(CHAR_UUID, BLERead | BLENotify, 64);

// Ultrasonic Read Function
float getDistanceCM(int trigPin, int echoPin) {
  // trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // measure echo
  long duration = pulseIn(echoPin, HIGH, 250); // timeout ~4.3m
  if (duration == 0) return -1;                  // no echo
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
      BLE.poll();
      // float left = getDistanceCM(LEFT_TRIG, LEFT_ECHO);
      // float right = getDistanceCM(RIGHT_TRIG, RIGHT_ECHO);
      float left = 0;
      float right = 0;

      digitalWrite(LED, (left>0 && left<15) || (right>0 && right<15));

      char jsonBuffer[80];
      snprintf(jsonBuffer, sizeof(jsonBuffer),
              "{\"left\":%.1f,\"right\":%.1f}", left, right);
      distChar.writeValue(jsonBuffer);   // auto-notifies
      delay(200);                        // give BLE stack time
    }
    Serial.println("Disconnected");
    BLE.advertise();
      }

      BLE.poll();
    }

