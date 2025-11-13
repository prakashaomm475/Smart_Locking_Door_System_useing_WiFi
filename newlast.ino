// __define-ocg__ SMART SOLENOID LOCK with ESP8266 + Keypad + Blynk + Serial
// Author: Omm Prakasha

#define BLYNK_TEMPLATE_ID "TMPL3ZcVPHmr5"
#define BLYNK_TEMPLATE_NAME "SMART DOOR LOCK"
#define BLYNK_AUTH_TOKEN "qZeUXKcePv5DRvfiFnNV1K07kKuJepUF"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Keypad.h>

// ---------- WiFi Credentials ----------
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "POCO X6 Neo 5G";        // 🔹 Enter your WiFi name
char pass[] = "8895847315";    // 🔹 Enter your WiFi password

// ---------- Pin Configuration ----------
#define RELAY D0       // Relay module connected to D1
#define VPIN_SWITCH V1 // Blynk switch virtual pin
#define VPIN_LED V2    // Blynk LED virtual pin

// ---------- Relay Auto-Off Timer ----------
#define RELAY_ON_TIME 5000  // 5 seconds (5000 ms)

// ---------- Password Settings ----------
String correctPassword = "7890";  // You can change your password here
String enteredPassword = "";

// ---------- Keypad Setup ----------
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {D1, D2, D3, D4};
byte colPins[COLS] = {D5, D6, D7, D8};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------- Variables ----------
bool relayState = false;
unsigned long relayStartTime = 0;

// ---------- Relay Control Functions ----------
void activateRelay() {
  digitalWrite(RELAY, HIGH);
  Blynk.virtualWrite(VPIN_LED, 255);
  Blynk.virtualWrite(VPIN_SWITCH, 1);
  Serial.println("🔓 Solenoid Lock: UNLOCKED (Relay ON)");
  relayState = true;
  relayStartTime = millis();
}

void deactivateRelay() {
  digitalWrite(RELAY, LOW);
  Blynk.virtualWrite(VPIN_LED, 0);
  Blynk.virtualWrite(VPIN_SWITCH, 0);
  Serial.println("🔒 Solenoid Lock: LOCKED (Relay OFF)");
  relayState = false;
}

// ---------- Blynk App Control ----------
BLYNK_WRITE(VPIN_SWITCH) {
  int value = param.asInt();
  if (value == 1) activateRelay();
  else deactivateRelay();
}

// ---------- Setup ----------
void setup() {
  Serial.begin(9600);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  
  Blynk.begin(auth, ssid, pass);
  Serial.println("\nSMART SOLENOID LOCK SYSTEM READY");
  Serial.println("Control by: Keypad | Serial Monitor | Blynk App");
  Serial.println("Serial Commands: Type ON or OFF");
}

// ---------- Main Loop ----------
void loop() {
  Blynk.run();

  // ---- Serial Control ----
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toUpperCase();

    if (input == "ON") {
      activateRelay();
    } else if (input == "OFF") {
      deactivateRelay();
    }
  }

  // ---- Keypad Control ----
  char key = keypad.getKey();
  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    if (key == '#') { // Enter key
      if (enteredPassword == correctPassword) {
        Serial.println("✅ Correct Password — Unlocking...");
        activateRelay();
      } else {
        Serial.println("❌ Wrong Password — Access Denied");
      }
      enteredPassword = ""; // Reset password
    } 
    else if (key == '*') { // Clear input
      enteredPassword = "";
      Serial.println("🔁 Password Cleared");
    } 
    else {
      enteredPassword += key;
      Serial.print("Entered: ");
      Serial.println(enteredPassword);
    }
  }

  // ---- Auto-Off after 5 seconds ----
  if (relayState && (millis() - relayStartTime >= RELAY_ON_TIME)) {
    deactivateRelay();
  }
}
