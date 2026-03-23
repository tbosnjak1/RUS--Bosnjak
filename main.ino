// LAB1 – ESP32 Prekidi s 3 LED, 2 tipkala i senzorom
#include <Arduino.h>

// LED pinovi
const int LED_GREEN = 2;
const int LED_YELLOW = 15;
const int LED_RED = 4;

// Tipkala
const int BUTTON_HIGH = 16; // tipka za ALARM
const int BUTTON_RESET = 17; // tipka za RESET

// HC-SR04 senzor
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;

// Timer varijabla
hw_timer_t *timer = NULL;
volatile int timerFlag = 0;

// Stanja
enum State {NORMAL, WARNING, ALARM};
volatile State systemState = NORMAL;

// Debounce
volatile unsigned long lastButtonHigh = 0;
volatile unsigned long lastButtonReset = 0;

// Interrupt handler za HIGH tipku
void IRAM_ATTR onHighButton() {
  unsigned long now = millis();
  if (now - lastButtonHigh > 200) { // debounce 200ms
    systemState = ALARM;
    Serial.println(">>> CRITICAL INTERRUPT (HIGH)");
    lastButtonHigh = now;
  }
}

// Interrupt handler za RESET tipku
void IRAM_ATTR onResetButton() {
  unsigned long now = millis();
  if (now - lastButtonReset > 200) {
    systemState = NORMAL;
    Serial.println(">>> RESET pressed, back to NORMAL");
    lastButtonReset = now;
  }
}

// Timer interrupt handler – generira flag svakih 500ms
void IRAM_ATTR onTimer() {
  timerFlag = 1;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("System ready.");

  // LED
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Tipkala
  pinMode(BUTTON_HIGH, INPUT_PULLUP);
  pinMode(BUTTON_RESET, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_HIGH), onHighButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RESET), onResetButton, FALLING);

  // HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Timer 500ms
  timer = timerBegin(0, 80, true); // 80 prescaler = 1us po tick
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 500000, true); // 500ms
  timerAlarmEnable(timer);
}

long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  long distance = duration / 58; // cm
  return distance;
}

void loop() {
  if (timerFlag) {
    timerFlag = 0;

    long dist = readDistance();
    Serial.print("Distance: "); Serial.println(dist);

    // Logika stanja
    switch (systemState) {
      case NORMAL:
        digitalWrite(LED_GREEN, !digitalRead(LED_GREEN)); // blink
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, LOW);
        if (dist < 10) systemState = WARNING;
        break;

      case WARNING:
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_YELLOW, HIGH);
        digitalWrite(LED_RED, LOW);
        break;

      case ALARM:
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, HIGH);
        break;
    }
  }
}
