#define LED_GREEN 2
#define LED_YELLOW 15
#define LED_RED 18

#define BUTTON_HIGH 4
#define BUTTON_RESET 5

#define TRIG_PIN 12
#define ECHO_PIN 14

// FLAGS
volatile bool highFlag = false;
volatile bool resetFlag = false;

// debounce
unsigned long lastHighPress = 0;
unsigned long lastResetPress = 0;
const int debounceDelay = 200;

// stanje sustava
enum State { NORMAL, WARNING, ALARM };
State currentState = NORMAL;

// ISR HIGH
void IRAM_ATTR highInterrupt() {
  unsigned long now = millis();
  if (now - lastHighPress > debounceDelay) {
    highFlag = true;
    lastHighPress = now;
  }
}

// ISR RESET
void IRAM_ATTR resetInterrupt() {
  unsigned long now = millis();
  if (now - lastResetPress > debounceDelay) {
    resetFlag = true;
    lastResetPress = now;
  }
}

// mjerenje udaljenosti
long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

unsigned long lastSensor = 0;

void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  pinMode(BUTTON_HIGH, INPUT_PULLUP);
  pinMode(BUTTON_RESET, INPUT_PULLUP);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_HIGH), highInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RESET), resetInterrupt, FALLING);

  Serial.println("System ready.");
}

void loop() {
  unsigned long now = millis();

  // --- HIGH PRIORITY ---
  if (highFlag) {
    highFlag = false;
    currentState = ALARM;
    Serial.println(">>> CRITICAL INTERRUPT (HIGH)");
  }

  // --- RESET ---
  if (resetFlag) {
    resetFlag = false;
    currentState = NORMAL;
    Serial.println(">>> RESET");
  }

  // --- SENSOR (LOW PRIORITY) ---
  if (now - lastSensor > 2000 && currentState != ALARM) {
    lastSensor = now;

    long dist = readDistance();

    Serial.print("Distance: ");
    Serial.println(dist);

    if (dist < 20) {
      currentState = WARNING;
    } else if (currentState == WARNING) {
      currentState = NORMAL;
    }
  }

  // --- LED CONTROL (STATE MACHINE) ---
  if (currentState == NORMAL) {
    digitalWrite(LED_GREEN, millis() % 1000 < 500); // blink
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  }

  if (currentState == WARNING) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
  }

  if (currentState == ALARM) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);
  }
}
