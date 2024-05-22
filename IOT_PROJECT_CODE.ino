#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3jrHhhPU2"
#define BLYNK_TEMPLATE_NAME "Water Level Management System"
#define BLYNK_AUTH_TOKEN "BqODyT55V5VaJh0H4JcBpTBhfmYdFryQ"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


#define trig 12
#define echo 13
#define relay 14

int MaxLevel = 13;

char auth[] = "BqODyT55V5VaJh0H4JcBpTBhfmYdFryQ";
char ssid[] = "Nuke";
char pass[] = "12345678";

int autom = 0;
bool manualMode = false;

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH); // Ensure the relay is off initially
}

void ultrasonic() {
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  int distance = t / 29 / 2;
  // Serial.print("og Reading: ");
  // Serial.println(distance);
  // Calculate Blynk display value
  int blynkDistance = (distance - MaxLevel) * -1;
  // Serial.print("Reading: ");
  // Serial.println(blynkDistance);
  
  if (blynkDistance < 6) {
    autom = 1;
    if (!manualMode) {
      digitalWrite(relay, LOW); // Turn on the pump if not in manual mode
      Serial.println("Pump ON (Auto)");
    }
  } else {
    autom = 0;
    if (!manualMode) {
      digitalWrite(relay, HIGH); // Turn off the pump if not in manual mode
      Serial.println("Pump OFF (Auto)");
    }
  }

  if (distance <= MaxLevel) {
    Blynk.virtualWrite(V0, blynkDistance);
  } else {
    Blynk.virtualWrite(V0, 0);
  }
}

BLYNK_WRITE(V1) {
  bool Relay = param.asInt();
  if (manualMode) {
    if (Relay == 1) {
      digitalWrite(relay, LOW); // Manually turn on the pump
      Serial.println("Pump ON (Manual)");
    } else {
      digitalWrite(relay, HIGH); // Manually turn off the pump
      Serial.println("Pump OFF (Manual)");
    }
  }
}

BLYNK_WRITE(V2) {
  manualMode = param.asInt();
  Serial.print("Manual Mode: ");
  Serial.println(manualMode);
}

void loop() {
  ultrasonic();
  Blynk.run();
}
