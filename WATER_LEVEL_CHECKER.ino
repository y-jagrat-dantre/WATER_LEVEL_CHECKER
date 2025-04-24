#include <WiFi.h>
#include <FirebaseESP32.h>

// Wi-Fi credentials
#define WIFI_SSID "ZTE-cz9JjE"
#define WIFI_PASSWORD "pgh3pt6r"

// Firebase config
#define FIREBASE_HOST "esp32-cam-python-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "jd04MpCh5jRrlrIfuI3LFVqgffengJMvIU4Ryynb"

FirebaseData firebaseData;

#define TRIG_PIN 27
#define ECHO_PIN 26

long duration;
float distanceCm;
int proximityPercent;

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConnected to Wi-Fi!");

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Trigger ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo time
  duration = pulseIn(ECHO_PIN, HIGH);

  // Convert to distance (in cm)
  distanceCm = duration * 0.034 / 2;

  // Cap to max distance range
  const int minDistance = 2;   // Closest, 100%
  const int maxDistance = 9; // Farthest, 0%
  
  // Clamp to valid range
  if (distanceCm < minDistance) distanceCm = minDistance;
  if (distanceCm > maxDistance) distanceCm = maxDistance;

  // Reverse map distance to percentage
  proximityPercent = map(distanceCm, maxDistance, minDistance, 0, 100);
  proximityPercent = constrain(proximityPercent, 0, 100);

  // Send to Firebase
  if (Firebase.setInt(firebaseData, "/waterLevel", proximityPercent)) {
    Serial.println("Sent to Firebase!");
  } else {
    Serial.print("Firebase Error: ");
    Serial.println(firebaseData.errorReason());
  }

  delay(1000);
}
