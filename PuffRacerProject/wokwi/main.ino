#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

#define POT_PIN     34   ///< Potenciometar - simulacija puhanja
#define LED_GREEN   26   ///< Zelena LED - razina 1 (600 ml/s)
#define LED_YELLOW  27   ///< Žuta LED - razina 2 (900 ml/s)
#define LED_RED     25   ///< Crvena LED - razina 3 (1200 ml/s)

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define FLOW_MIN      0      ///< Minimalni protok (ml/s)
#define FLOW_MAX      1400   ///< Maksimalni protok (ml/s)
#define THRESHOLD_1   600    ///< Prag razine 1 (ml/s)
#define THRESHOLD_2   900    ///< Prag razine 2 (ml/s)
#define THRESHOLD_3   1200   ///< Prag razine 3 - KAZNA (ml/s)
#define SUCCESS_TIME  5000   ///< Trajanje uspješne vježbe (ms)
#define EMA_ALPHA     0.2    ///< Koeficijent EMA filtriranja (0-1)
#define SAMPLE_MS     50     ///< Interval uzorkovanja (ms)

enum State { IDLE, RUNNING, SUCCESS, PENALTY };
State currentState = IDLE;

float emaFlow        = 0;   ///< EMA filtrirani protok
float currentFlow    = 0;   ///< Trenutni protok (ml/s)
float bestResult     = 0;   ///< Najbolji rezultat

unsigned long successStart = 0; ///< Početak uspješne zone
unsigned long successTime  = 0; ///< Akumulirano uspješno vrijeme
unsigned long lastSample   = 0; ///< Timestamp zadnjeg uzorkovanja
unsigned long penaltyStart = 0; ///< Početak penalty stanja

Preferences prefs; 

float readFlow() {
  int raw = analogRead(POT_PIN);
  float flow = map(raw, 0, 4095, FLOW_MIN, FLOW_MAX);
  emaFlow = EMA_ALPHA * flow + (1 - EMA_ALPHA) * emaFlow;
  return emaFlow;
}

void updateLEDs(float flow) {
  digitalWrite(LED_GREEN,  flow >= THRESHOLD_1 ? HIGH : LOW);
  digitalWrite(LED_YELLOW, flow >= THRESHOLD_2 ? HIGH : LOW);
  digitalWrite(LED_RED,    flow >= THRESHOLD_3 ? HIGH : LOW);
}

void saveBestResult(float result) {
  prefs.begin("puffracer", false);
  prefs.putFloat("best", result);
  prefs.end();
  Serial.print("[NVS] Spremljen novi rekord: ");
  Serial.println(result);
}

float loadBestResult() {
  prefs.begin("puffracer", true);
  float best = prefs.getFloat("best", 0);
  prefs.end();
  return best;
}

void drawDisplay(float flow, float timeLeft) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("PuffRacer F1");

  display.setCursor(0, 12);
  display.print("Protok: ");
  display.print((int)flow);
  display.println(" ml/s");

  int barWidth = map(constrain(flow, 0, FLOW_MAX), 0, FLOW_MAX, 0, 100);
  display.drawRect(0, 25, 100, 10, SSD1306_WHITE);
  display.fillRect(0, 25, barWidth, 10, SSD1306_WHITE);

  display.drawLine(42, 23, 42, 37, SSD1306_WHITE); // 600
  display.drawLine(64, 23, 64, 37, SSD1306_WHITE); // 900
  display.drawLine(85, 23, 85, 37, SSD1306_WHITE); // 1200

  display.setCursor(0, 40);
  switch (currentState) {
    case IDLE:
      display.println("Okreni pot. za start!");
      break;
    case RUNNING:
      display.print("Vrijeme: ");
      display.print(timeLeft / 1000.0, 1);
      display.println("s");
      break;
    case SUCCESS:
      display.println(">> POBJEDA! <<");
      break;
    case PENALTY:
      display.println("!! PREBRZO !!");
      break;
  }

  display.setCursor(0, 54);
  display.print("Rekord: ");
  display.print((int)bestResult);
  display.println(" ml/s");

  display.display();
}


void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED,    OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("[ERROR] OLED nije pronađen!");
    while (true);
  }
  display.clearDisplay();
  display.display();

  bestResult = loadBestResult();

  Serial.println("=== PuffRacer F1 ===");
  Serial.print("[NVS] Učitan rekord: ");
  Serial.println(bestResult);
  Serial.println("Okreni potenciometar za početak!");
}


void loop() {
  unsigned long now = millis();

  if (now - lastSample < SAMPLE_MS) return;
  lastSample = now;

  currentFlow = readFlow();
  updateLEDs(currentFlow);

  switch (currentState) {

    case IDLE:
      successTime  = 0;
      successStart = 0;
      if (currentFlow > THRESHOLD_1) {
        currentState = RUNNING;
        Serial.println("[START] Vježba započeta!");
      }
      break;

    case RUNNING: {
      static unsigned long lastPrint = 0;
      if (now - lastPrint >= 1000) {
        lastPrint = now;
        Serial.print("[FLOW] ");
        Serial.print((int)currentFlow);
        Serial.println(" ml/s");
      }

      if (currentFlow >= THRESHOLD_3) {
        currentState = PENALTY;
        penaltyStart = now;
        successTime  = 0;
        Serial.println("[KAZNA] Prebrzo! Vježba poništena.");
        break;
      }

      if (currentFlow >= THRESHOLD_2 && currentFlow < THRESHOLD_3) {
        if (successStart == 0) successStart = now;
        successTime = now - successStart;

        if (successTime >= SUCCESS_TIME) {
          currentState = SUCCESS;
          if (currentFlow > bestResult) {
            bestResult = currentFlow;
            saveBestResult(bestResult);
          }
          Serial.println("[USPJEH] Vježba uspješno završena!");
        }
      } else {
        successStart = 0;
        successTime  = 0;
      }
      break;
    }

    case SUCCESS:
      if (now - successStart > 3000) {
        currentState = IDLE;
        Serial.println("[RESET] Novi pokušaj...");
      }
      break;

    case PENALTY:
      if (penaltyStart == 0) penaltyStart = now;
      if (now - penaltyStart >= 2000) {
        penaltyStart = 0;
        currentState = IDLE;
        Serial.println("[RESET] Pokušaj ponovo.");
      }
      break;
  }

  drawDisplay(currentFlow, successTime);
}
