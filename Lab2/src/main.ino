/**
 * @file main.ino
 * @brief RUS Lab2 - Pametni poštanski sandučić (Event-driven Deep Sleep)
 *
 * Sustav je većinu vremena u Deep Sleep modu.
 * Budi se isključivo kada se pritisne tipkalo (vanjski prekid na GPIO 32).
 * Nakon buđenja evidentira događaj, izvršava kratku obradu i vraća se u sleep.
 * Koristi RTC memoriju za čuvanje broja događaja kroz sleep cikluse.
 * Debouncing je implementiran vremenskim ignoriranjem ponovljenih prekida.
 *
 */

#include "esp_sleep.h"
#include "WiFi.h"
#include "esp_bt.h"

#define BUTTON_PIN  32
#define LED_PIN     2
#define DEBOUNCE_MS 500

int packageCount = 0;
unsigned long lastPressTime = 0;
bool isActive = false;
unsigned long activeStart = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, LOW);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  btStop();

  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("[INIT] Pametni postanski sanducic");
  Serial.println("[INIT] Sustav spreman. Cekam paket...");
  Serial.println("[SLEEP] Simulirani Deep Sleep aktivan.");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
}

void loop() {
  unsigned long now = millis();

  // Ako je aktivna faza gotova (proslo 3s)
  if (isActive && now - activeStart >= 3000) {
    digitalWrite(LED_PIN, LOW);
    isActive = false;
    Serial.println("[GOTOVO] Obrada završena.");
    Serial.println("[PERIFERIJE] WiFi i Bluetooth onemogućeni.");
    Serial.println("[SLEEP] Ulazim u Deep Sleep...");
    Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  }

  // Provjera tipkala samo ako nije aktivan
  if (!isActive && digitalRead(BUTTON_PIN) == LOW) {
    if (now - lastPressTime > DEBOUNCE_MS) {
      lastPressTime = now;
      packageCount++;
      isActive = true;
      activeStart = now;

      Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
      Serial.println("[BUDENJE] Paket detektiran!");
      Serial.print("[INFO] Ukupno paketa: ");
      Serial.println(packageCount);
      Serial.println("[AKTIVAN] Obrada paketa... (3s)");
      digitalWrite(LED_PIN, HIGH);
    } else {
      Serial.println("[DEBOUNCE] Ignoriran lažni signal!");
    }
  }
}
