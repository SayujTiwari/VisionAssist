#define TRIG1 2
#define ECHO1 3
#define TRIG2 4
#define ECHO2 5
#define LED   13

long readDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 30000); // timeout 30 ms
  if (duration == 0) return -1;               // no echo
  return (duration * 0.0343) / 2;             // cm
}

void setup() {
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  Serial.println("Starting distance test...");
}

void loop() {
  long d1 = readDistance(TRIG1, ECHO1);
  long d2 = readDistance(TRIG2, ECHO2);

  Serial.print("Front: ");
  Serial.print(d1);
  Serial.print(" cm\tDown: ");
  Serial.print(d2);
  Serial.println(" cm");

  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(400);
}
