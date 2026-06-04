# Korisnički priručnik — PuffRacer F1

## Dobrodošli u PuffRacer! 🏎️

PuffRacer je simulacija medicinskog spirometra u obliku F1 utrke.
Vaš zadatak je kontrolirati protok daha i zadržati ga u optimalnoj
zoni točno 5 sekundi kako biste pobijedili!

---

## Potrebna oprema

- ESP32 razvojna pločica
- Potenciometar (simulacija puhanja)
- OLED zaslon SSD1306
- 3 LED diode (zelena, žuta, crvena)
- Wokwi simulator: [Otvori projekt](https://wokwi.com/projects/465526973084188673)

---

## Pokretanje simulacije

1. Otvori [Wokwi projekt](https://wokwi.com/projects/465526973084188673)
2. Klikni gumb **Play ▶** za pokretanje
3. Otvori **Serial Monitor** za praćenje ispisa
4. Okreći potenciometar za simulaciju puhanja

---

## Kako igrati?

### Korak 1 — Pokretanje
Okreni potenciometar u desno dok protok ne prijeđe **600 ml/s**.
Zelena LED će se upaliti i vježba počinje!

### Korak 2 — Optimalna zona
Zadržaj potenciometar u položaju gdje protok iznosi između
**900 i 1200 ml/s**. Žuta LED će se upaliti.

### Korak 3 — Pobjeda
Zadržaj optimalnu zonu neprekidno **5 sekundi** →
OLED prikazuje **">> POBJEDA! <<"** 🏆

---

## LED indikatori

| LED | Boja | Značenje | Protok |
|-----|------|----------|--------|
| LED 1 | 🟢 Zelena | Vježba aktivna | > 600 ml/s |
| LED 2 | 🟡 Žuta | Optimalna zona | > 900 ml/s |
| LED 3 | 🔴 Crvena | KAZNA! | > 1200 ml/s |

---

## OLED zaslon
┌─────────────────────────┐
│ PuffRacer F1            │
│ Protok: 950 ml/s        │
│ ████████░░░░ [bar]      │
│ Vrijeme: 3.2s           │
│ Rekord: 1050 ml/s       │
└─────────────────────────┘
| Element | Opis |
|---------|------|
| Protok | Trenutni izmjereni protok u ml/s |
| Bar indikator | Vizualni prikaz protoka s pragovima |
| Vrijeme | Koliko sekundi ste u optimalnoj zoni |
| Rekord | Vaš najbolji rezultat (sprema se trajno) |

---

## Stanja sustava

| Stanje | Opis | OLED poruka |
|--------|------|-------------|
| IDLE | Čeka početak | "Okreni pot. za start!" |
| RUNNING | Vježba u tijeku | "Vrijeme: X.Xs" |
| SUCCESS | Pobjeda! | ">> POBJEDA! <<" |
| PENALTY | Prebrzo puhanje | "!! PREBRZO !!" |

---

## Savjeti za igru 💡

- **Previše sporo** (< 600 ml/s) → auto ne kreće, okreni više
- **Prebrzo** (> 1200 ml/s) → KAZNA, vježba se poništava
- **Optimalno** (900-1200 ml/s) → zadržite 5 sekundi za pobjedu
- Vaš rekord se **automatski sprema** i učitava pri sljedećem pokretanju

---

## Medicinska pozadina

PuffRacer je inspiriran medicinskim poticajnim spirometrom koji se
koristi u rehabilitaciji pluća. Pacijenti moraju udisati stabilno
i kontrolirano — presnažan udah uzrokuje turbulenciju i nije
terapeutski koristan. Naš F1 scenarij to preslikava u:

- **Premalo goriva** = presporo puhanje
- **Optimalan protok** = savršen pit-stop
- **Prolijevanje goriva** = prebrzo puhanje → KAZNA

---

## Rješavanje problema

| Problem | Rješenje |
|---------|----------|
| OLED ne prikazuje ništa | Provjeri I2C adresu (0x3C) |
| LED ne reagira | Provjeri spajanje GPIO pinova |
| Potenciometar ne radi | Provjeri GPIO 34 (ADC pin) |
| Simulacija ne pokreće | Pokušaj osvježiti stranicu (F5) |

---

## Autori

**Toma Bošnjak & Borna Šafar**
RUS — Razvoj ugradbenih sustava | TVZ Zagreb 2026.
