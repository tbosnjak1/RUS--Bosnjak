/**
 * @file sketch.ino
 * @brief RUS Lab1 - Upravljanje višestrukim prekidima na ESP32
 *
 * @mainpage RUS Lab1: Višestruki prekidi i prioritizacija na ESP32
 *
 * @section opis Opis projekta
 * Demonstracija višestrukih prekida s definiranom hijerarhijom prioriteta.
 * Koriste se 3 neovisna izvora prekida:
 *  - Hardware Timer0 (najviši prioritet) - okida se svakih 2 sekunde
 *  - Tipkalo BUTTON_HIGH (visoki prioritet) - forsira ALARM stanje
 *  - Tipkalo BUTTON_RESET (niski prioritet) - resetira sustav
 *
 * Senzor HC-SR04 koristi polling (srednji prioritet u loop-u).
 *
 * @section prioriteti Hijerarhija prioriteta
 * | Prioritet | Izvor         | Akcija                    |
 * |-----------|---------------|---------------------------|
 * | 1-NAJVIŠI | Timer0 ISR    | Heartbeat log svake 2s    |
 * | 2-VISOKI  | BUTTON_HIGH   | Prebaci u ALARM           |
 * | 3-SREDNJI | HC-SR04       | WARNING ili ALARM po dist.|
 * | 4-NISKI   | BUTTON_RESET  | Vrati u NORMAL            |
 *
 * @section stanja Stanja sustava
 * - NORMAL  -> zelena LED trepce
 * - WARNING -> zuta LED upaljena (objekt < 20cm)
 * - ALARM   -> crvena LED upaljena (tipkalo ili objekt < 10cm)
 *
 * @author Bosnjak
 * @date Ozujak 2025.
 * @version 2.0
 */

#define LED_GREEN   2
#define LED_YELLOW  15
#define LED_RED     18

#define BUTTON_HIGH  4
#define BUTTON_RESET 5

#define TRIG_PIN 12
#define ECHO_PIN 14

#define DEBOUNCE_MS      200
#define SENSOR_INTERVAL  500
#define TIMER_INTERVAL   2000000
#define DIST_WARNING     20
#define DIST_ALARM       10

/**
 * @brief Enumeracija stanja sustava koristena u state machine logici.
 */
enum State { NORMAL, WARNING, ALARM };
volatile State currentState = NORMAL;

volatile bool timerFlag  = false;
volatile bool highFlag   = false;
volatile bool resetFlag  = false;
volatile int  timerCount = 0;

volatile unsigned long lastHighPress  = 0;
volatile unsigned long lastResetPress = 0;
unsigned long lastSensor = 0;

/**
 * @brief Mutex za zastitu dijeljenih varijabli izmedju ISR-ova i loop()-a.
 *
 * ESP32 podrzava nested interrupts - dva ISR-a mogu prekinuti jedan drugog.
 * portENTER_CRITICAL_ISR/portEXIT_CRITICAL_ISR osiguravaju atomarno
 * postavljanje zastavica bez race conditiona.
 */
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

hw_timer_t *timer0 = NULL;

/**
 * @brief ISR za Hardware Timer0 - NAJVISI prioritet.
 *
 * Okida se svakih 2 sekunde. Koristi portENTER_CRITICAL_ISR za
 * atomarno postavljanje zastavice. IRAM_ATTR - izvrsava se iz RAM-a.
 */
void IRAM_ATTR timerISR() {
  portENTER_CRITICAL_ISR(&mux);
  timerFlag = true;
  timerCount++;
  portEXIT_CRITICAL_ISR(&mux);
}

/**
 * @brief ISR za tipkalo BUTTON_HIGH - VISOKI prioritet.
 *
 * Okida se na padajuci brid (FALLING). Provodi debounce provjeru.
 * Postavlja highFlag koji loop() obradjuje kao prijelaz u ALARM.
 *
 * @note IRAM_ATTR je obavezan na ESP32 za sve ISR funkcije.
 */
void IRAM_ATTR isrButtonHigh() {
  unsigned long now = millis();
  if (now - lastHighPress > DEBOUNCE_MS) {
    portENTER_CRITICAL_ISR(&mux);
    highFlag = true;
    lastHighPress = now;
    portEXIT_CRITICAL_ISR(&mux);
  }
}

/**
 * @brief ISR za tipkalo BUTTON_RESET - NISKI prioritet.
 *
 * Okida se na padajuci brid (FALLING). Provodi debounce provjeru.
 * Postavlja resetFlag koji loop() obradjuje kao povratak u NORMAL.
 */
void IRAM_ATTR isrButtonReset() {
  unsigned long now = millis();
  if (now - lastResetPress > DEBOUNCE_MS) {
    portENTER_CRITICAL_ISR(&mux);
    resetFlag = true;
    lastResetPress = now;
    portEXIT_CRITICAL_ISR(&mux);
  }
}

/**
 * @brief Mjeri udaljenost pomocu HC-SR04 ultrazvucnog senzora.
 *
 * Salje 10us impuls na TRIG i mjeri trajanje ECHO pulsa.
 * Formula: udaljenost = (trajanje * 0.034) / 2
 *
 * @return Udaljenost u cm, ili 999 ako mjerenje nije uspjelo.
 */
long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

/**
 * @brief Azurira LED diode prema trenutnom stanju state machine-a.
 *
 * - NORMAL  : zelena trepce (500ms period)
 * - WARNING : zuta upaljena, ostale ugasene
 * - ALARM   : crvena upaljena, ostale ugasene
 */
void updateLEDs() {
  switch (currentState) {
    case NORMAL:
      digitalWrite(LED_GREEN,  (millis() % 1000) < 500);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED,    LOW);
      break;
    case WARNING:
      digitalWrite(LED_GREEN,  LOW);
      digitalWrite(LED_YELLOW, HIGH);
      digitalWrite(LED_RED,    LOW);
      break;
    case ALARM:
      digitalWrite(LED_GREEN,  LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED,    HIGH);
      break;
  }
}

/**
 * @brief Obradjuje sve aktivirane prekide prema definiranoj hijerarhiji.
 *
 * Redoslijed provjere zastavica odgovara prioritetima:
 *  1. timerFlag  - NAJVISI: heartbeat ispis
 *  2. highFlag   - VISOKI:  prebacuje u ALARM
 *  3. resetFlag  - NISKI:   vraca u NORMAL
 */
void handleInterrupts() {
  if (timerFlag) {
    portENTER_CRITICAL(&mux);
    timerFlag = false;
    int cnt = timerCount;
    portEXIT_CRITICAL(&mux);
    Serial.print("[TIMER P1] Heartbeat #");
    Serial.print(cnt);
    Serial.print(" | Stanje: ");
    Serial.println(currentState == NORMAL  ? "NORMAL"  :
                   currentState == WARNING ? "WARNING" : "ALARM");
  }

  if (highFlag) {
    portENTER_CRITICAL(&mux);
    highFlag = false;
    portEXIT_CRITICAL(&mux);
    currentState = ALARM;
    Serial.println("[HIGH  P2] ALARM aktiviran tipkalom!");
  }

  if (resetFlag) {
    portENTER_CRITICAL(&mux);
    resetFlag = false;
    portEXIT_CRITICAL(&mux);
    currentState = NORMAL;
    Serial.println("[RESET P4] Sustav resetiran u NORMAL.");
  }
}

/**
 * @brief Inicijalizacija sustava - izvrsava se jednom pri pokretanju.
 *
 * Konfigurira GPIO pinove, vanjske prekide i Hardware Timer0.
 * Timer frekvencija: 1MHz = 1 tick svaka mikrosekunda.
 * Alarm na 2.000.000 tickova = tocno 2 sekunde, autoreload=true.
 */
void setup() {
  Serial.begin(115200);
  Serial.println("=== RUS Lab1 - Visestruki prekidi ESP32 ===");
  Serial.println("Prioriteti: Timer(P1) > ButtonHIGH(P2) > Senzor(P3) > ButtonRESET(P4)");

  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED,    OUTPUT);
  pinMode(BUTTON_HIGH,  INPUT_PULLUP);
  pinMode(BUTTON_RESET, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_HIGH),  isrButtonHigh,  FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RESET), isrButtonReset, FALLING);

  timer0 = timerBegin(1000000);
  timerAttachInterrupt(timer0, &timerISR);
  timerAlarm(timer0, TIMER_INTERVAL, true, 0);

  Serial.println("Inicijalizacija zavrsena. Cekam dogadjaje...");
}

/**
 * @brief Glavna programska petlja.
 *
 * Redoslijed izvrsavanja u svakoj iteraciji:
 *  1. handleInterrupts() - obrada zastavica po prioritetu
 *  2. Ocitavanje senzora svakih 500ms (polling, P3)
 *  3. updateLEDs() - prikaz trenutnog stanja
 *
 * @note pulseIn() nije siguran unutar ISR konteksta pa senzor
 *       koristi polling pristup umjesto prekida.
 */
void loop() {
  unsigned long now = millis();

  handleInterrupts();

  if (now - lastSensor > SENSOR_INTERVAL && currentState != ALARM) {
    lastSensor = now;
    long dist = readDistance();
    Serial.print("[SENSOR P3] Udaljenost: ");
    Serial.print(dist);
    Serial.println(" cm");

    if (dist < DIST_ALARM) {
      currentState = ALARM;
      Serial.println("[SENSOR P3] ALARM - objekt preblizu (<10cm)!");
    } else if (dist < DIST_WARNING) {
      currentState = WARNING;
      Serial.println("[SENSOR P3] WARNING - objekt u blizini (<20cm).");
    } else if (currentState == WARNING) {
      currentState = NORMAL;
      Serial.println("[SENSOR P3] NORMAL - slobodan put.");
    }
  }

  updateLEDs();
}
