#include <ArduinoBLE.h>
#include <Servo.h>

// === Pin layout ===
#define OBST_TRIG 5
#define OBST_ECHO 7
#define TRENCH_TRIG 6
#define TRENCH_ECHO 8
#define BUZZER 4
#define SERVO_PIN 10

// === BLE setup ===
#define SERVICE_UUID "c1d0a000-1234-4abc-bbbb-1234567890ab"
#define CHAR_UUID    "c1d0a001-1234-4abc-bbbb-1234567890ab"
BLEService visionService(SERVICE_UUID);
BLEStringCharacteristic distChar(CHAR_UUID, BLERead | BLENotify, 128);

Servo scanServo;

// === Constants ===
const float alpha = 0.2;       // smoothing factor
const float mountHeight = 1.0; // meters
const float obstacleLimit = 80.0; // meters
const float trenchLimit = 110.0;   // cm

// === Globals ===
float obstSmooth = 0;
float trenchSmooth = 0;

// === Distance function (non-blocking) ===
float getDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long start = micros();
  while (digitalRead(echoPin) == LOW) {
    BLE.poll();
    if (micros() - start > 30000) return -1;
  }

  unsigned long echoStart = micros();
  while (digitalRead(echoPin) == HIGH) {
    BLE.poll();
    if (micros() - echoStart > 30000) return -1;
  }

  unsigned long duration = micros() - echoStart;
  return duration * 0.0343 / 2.0; // cm
}

// === Setup ===
void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(OBST_TRIG, OUTPUT);
  pinMode(OBST_ECHO, INPUT);
  pinMode(TRENCH_TRIG, OUTPUT);
  pinMode(TRENCH_ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);

  scanServo.attach(SERVO_PIN);
  scanServo.write(150);

  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }

  BLE.setLocalName("VisionAssist");
  BLE.setAdvertisedService(visionService);
  visionService.addCharacteristic(distChar);
  BLE.addService(visionService);

  distChar.writeValue("{\"status\":\"ready\"}");
  BLE.advertise();
  Serial.println("Advertising as VisionAssist...");
}

// === Main loop ===
void loop() {
  static int angle = 150;
  static int step = -5;

  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to: ");
    Serial.println(central.address());

    while (central.connected()) {
      // === Servo oscillation ===
      scanServo.write(angle);
      angle += step;
      if (angle <= 30 || angle >= 150) step = -step;

      // === Distance readings ===
      float obstRaw = getDistanceCM(OBST_TRIG, OBST_ECHO);
      float trenchRaw = getDistanceCM(TRENCH_TRIG, TRENCH_ECHO);

      if (obstSmooth == 0 && obstRaw > 0) obstSmooth = obstRaw;
      if (trenchSmooth == 0 && trenchRaw > 0) trenchSmooth = trenchRaw;

      if (obstRaw > 0) obstSmooth = alpha * obstRaw + (1 - alpha) * obstSmooth;
      if (trenchRaw > 0) trenchSmooth = alpha * trenchRaw + (1 - alpha) * trenchSmooth;

      // === Trigonometric correction ===
      float thetaRad = radians(angle);
      float hyp = obstSmooth / 100.0; // convert cm → m
      float horiz = hyp * cos(thetaRad - PI / 2.0);
      float height = mountHeight + hyp * sin(thetaRad - PI / 2.0);
      bool valid = (height > 0);

      // === Logic ===
      bool obstacle = (valid && horiz > 0 && horiz < obstacleLimit);
      bool trench = (trenchSmooth > trenchLimit);

      // === Buzzer alert ===
      if (obstacle || trench) {
        tone(BUZZER, 1000); // 1 kHz tone
      } else {
        noTone(BUZZER);
      }

      // === BLE JSON output ===
      char jsonBuffer[128];
      snprintf(jsonBuffer, sizeof(jsonBuffer),
        "{\"angle\":%d,\"sensorObstacleRaw\":%.2f,\"horizontal\":%.2f,"
        "\"height\":%.2f,\"sensorTrench\":%.2f}",
        angle, obstSmooth, horiz, height, trenchSmooth);

      distChar.writeValue(jsonBuffer);
      Serial.println(jsonBuffer);

      BLE.poll();
      delay(200);
    }

    Serial.println("Disconnected");
    noTone(BUZZER);
    BLE.advertise();
  }

  BLE.poll();
}
