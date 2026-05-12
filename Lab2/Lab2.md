# RUS Lab2 — Pametni poštanski sandučić (Event-driven Deep Sleep)

> **Predmet:** Razvoj ugradbenih sustava (RUS)  
> **Autor:** Bosnjak  
> **Platforma:** ESP32 (Wokwi simulator)  
> **Varijanta:** A — Pametni poštanski sandučić  
> **Simulacija:** (https://wokwi.com/projects/463348291726456833)

---

## Kratki opis rješenja

Sustav simulira pametni poštanski sandučić koji je većinu vremena u **Deep Sleep** modu. Budi se isključivo kada se pritisne tipkalo (vanjski prekid na GPIO 32), što simulira ubacivanje paketa u sandučić. Nakon buđenja evidentira događaj, pali LED 3 sekunde kao simulaciju obrade, te se vraća u Deep Sleep.

Broj primljenih paketa čuva se u **RTC memoriji** i preživljava Deep Sleep cikluse.

---

## Sažetak implementacije

| Stavka | Odgovor |
|--------|---------|
| Platforma | ESP32 |
| Varijanta | A — Pametni poštanski sandučić |
| Sleep mode | Deep Sleep (`esp_deep_sleep_start()`) |
| Buđenje | Vanjski prekid EXT0 na GPIO 32 |
| Čuvanje stanja | RTC memorija (`RTC_DATA_ATTR`) |
| Debouncing | Vremensko ignoriranje (300ms) |
| Wokwi link | [Otvori projekt]https://wokwi.com/projects/463721164653718529 |

---

## Komponente

| Komponenta | Pin | Opis |
|------------|-----|------|
| LED crvena | GPIO 2 | Indikator aktivne faze |
| Tipkalo | GPIO 32 | Buđenje iz Deep Sleepa |
| Otpornik 220Ω | - | Zaštita LED diode |

---

## Napomena o simulaciji

Wokwi simulator omogućuje testiranje logike sleep/wake ciklusa, ali ne simulira stvarnu potrošnju energije. Implementacija prikazuje logiku upravljanja energijom, ali ne omogućuje stvarnu procjenu potrošnje energije.
