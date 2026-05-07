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

#define BUTTON_PIN  32  ///< Pin tipkala za buđenje
#define LED_PIN     2   ///< LED indikator aktivne faze
#define DEBOUNCE_MS 300 ///< Debounce period u ms

RTC_DATA_ATTR int packageCount = 0;           ///< Broj paketa - čuva se kroz Deep Sleep
RTC_DATA_ATTR unsigned long lastWakeTime = 0; ///< Timestamp za debouncing

/**
 * @brief Aktivna faza - izvršava se nakon buđenja.
 *
 * Evidentira novi paket, uključuje LED na 3 sekunde
 * kao simulaciju obrade događaja, zatim gasi LED.
 */
void activeFase() {
  packageCount++;
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("[BUDENJE] Paket detektiran!");
  Serial.print("[INFO] Ukupno paketa: ");
  Serial.println(packageCount);
  Serial.println("[AKTIVAN] Obrada paketa... (3s)");
  digitalWrite(LED_PIN, HIGH);
  delay(3000);
  digitalWrite(LED_PIN, LOW);
  Serial.println("[GOTOVO] Obrada završena.");
}

/**
 * @brief Onemogućava nepotrebne periferije prije ulaska u sleep.
 *
 * Isključuje WiFi i Bluetooth kako bi se minimizirala
 * potrošnja energije tijekom Deep Sleep faze.
 */
void disablePeripherals() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  btStop();
  Serial.println("[PERIFERIJE] WiFi i Bluetooth onemogućeni.");
}

/**
 * @brief Ulazak u Deep Sleep mod.
 *
 * Prije spavanja onemogućava periferije, konfigurira
 * buđenje putem EXT0 prekida na BUTTON_PIN (LOW razina).
 */
void enterSleep() {
  disablePeripherals();
  Serial.println("[SLEEP] Ulazim u Deep Sleep...");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.flush();
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);
  esp_deep_sleep_start();
}

/**
 * @brief Debounce provjera - vremensko ignoriranje ponovljenih prekida.
 *
 * @return true ako je događaj valjan, false ako je šum
 */
bool isValidWakeup() {
  unsigned long now = millis();
  if (now - lastWakeTime < DEBOUNCE_MS && lastWakeTime != 0) {
    Serial.println("[DEBOUNCE] Ignoriran lažni signal!");
    return false;
  }
  lastWakeTime = now;
  return true;
}

/**
 * @brief Inicijalizacija - izvršava se pri svakom buđenju iz Deep Sleepa.
 *
 * ESP32 nakon Deep Sleepa ne nastavlja od mjesta gdje je stao
 * nego izvršava setup() iznova. Stanje se čuva u RTC memoriji.
 */
void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(LED_PIN,    OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, LOW);

  esp_sleep_wakeup_cause_t wakeupReason = esp_sleep_get_wakeup_cause();

  if (wakeupReason == ESP_SLEEP_WAKEUP_EXT0) {
    if (isValidWakeup()) {
      activeFase();
    }
  } else {
    Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    Serial.println("[INIT] Pametni postanski sanducic");
    Serial.println("[INIT] Sustav spreman.");
    Serial.print("[INFO] Paketa do sada: ");
    Serial.println(packageCount);
    Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  }

  enterSleep();
}

/**
 * @brief Glavna petlja - nikad se ne izvršava.
 *
 * ESP32 u Deep Sleep modu uvijek restarta kroz setup().
 */
void loop() {
  // Nikad se ne izvršava - ESP32 ide u Deep Sleep iz setup()
}
