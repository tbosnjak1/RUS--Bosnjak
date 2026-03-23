# Lab1 – Prekidi u ugradbenim sustavima

## Opis projekta
Ovaj projekt koristi ESP32 mikrokontroler za demonstraciju rada s višestrukim prekidima. Sustav reagira na tipkala, tajmer i senzor udaljenosti te kontrolira tri LED diode.

## Cilj
- Razumjeti prekide mikrokontrolera i njihovu prioritizaciju
- Implementirati višestruke prekide s različitim prioritetima
- Testirati sustav u Wokwi simulatoru i dokumentirati sve korake

## Korištene komponente
- ESP32
- 3 LED diode (zelena, žuta, crvena)
- 2 tipkala
- HC-SR04 senzor udaljenosti
- Otpornici 220Ω za LED

## Opis rada sustava

Sustav funkcionira kao state machine:

### NORMAL
- Zelena LED blink
- Timer redovito mijenja stanje LED
- Senzor očitava udaljenost (svake sekunde)

### WARNING
- Žuta LED pali se kada je objekt bliže od 10 cm
- Event detektira senzor

### ALARM
- Crvena LED pali se pritiskom HIGH tipke
- Interrupt ima najveći prioritet
- Ostala logika se ignorira dok ne dođe reset

### RESET
- Tipkalo reset vraća stanje u NORMAL
- Žuta ili crvena LED se gase

## Wokwi simulacija
[OVDJE ZALIJEPI LINK SA SIMULACIJE]

## Doxygen dokumentacija
Dokumentacija koda generirana pomoću Doxygen alata nalazi se u direktoriju `docs/html`.  
Link do početne stranice generirane dokumentacije: `docs/html/index.html`

## Control Flow Graph (CFG)
```mermaid
graph TD
    A[Start loop] --> B[Check HIGH interrupt]
    B -->|Yes| C[Set ALARM]
    B -->|No| D[Check RESET]
    D -->|Yes| E[Set NORMAL]
    D -->|No| F[Check sensor]
    F -->|Close object| G[Set WARNING]
    F -->|Far| H[Set NORMAL]
    G --> I[Update LEDs]
    H --> I
    C --> I
    E --> I
