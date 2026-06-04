# PuffRacer – Spirometar za kontrolu trkačeg auta

# Opis projekta
Ovaj projekt je rezultat timskog rada u sklopu projektnog zadatka kolegija 
Razvoj ugradbenih sustava na Tehničkom veleučilištu u Zagrebu.

PuffRacer simulira medicinski spirometar u obliku F1 utrke. Korisnik okreće 
potenciometar (simulacija puhanja) i pokušava zadržati protok daha u optimalnoj 
zoni između 900 i 1200 ml/s tijekom 5 sekundi. Motivacija projekta je spojiti 
medicinsku rehabilitaciju pluća s gamifikacijom kroz F1 scenarij.

# Funkcijski zahtjevi
- Očitavanje potenciometra kao simulacije protoka daha (ADC, 12-bit)
- EMA filtriranje signala za zaglađivanje
- Prikaz 3 razine protoka na OLED zaslonu (600/900/1200 ml/s)
- LED indikatori razine protoka (zelena/žuta/crvena)
- Logika vježbe s uvjetom trajanja 5 sekundi
- Detekcija kazne ako protok prijeđe 1200 ml/s
- Spremanje najboljeg rezultata u NVS memoriju

# Tehnologije
- **Platforma:** ESP32
- **Simulator:** Wokwi
- **Framework:** Arduino
- **Biblioteke:** Adafruit SSD1306, Adafruit GFX, Preferences (NVS)
- **Jezik:** C++

# Instalacija
1. Otvori [Wokwi simulator](https://wokwi.com/projects/465526973084188673)
2. Pokreni simulaciju klikom na Play ▶
3. Okreći potenciometar za simulaciju puhanja
4. Zadržaj protok između 900-1200 ml/s kroz 5 sekundi za pobjedu

# Članovi tima
| Ime | GitHub | Doprinos |
|-----|--------|----------|
| Toma Bošnjak | [@tbosnjak1](https://github.com/tbosnjak1) | Kod, dokumentacija, simulacija |
| Borna Šafar | - | Dizajn, testiranje |

# Kontribucije
Projekt je razvijen u sklopu kolegija RUS na TVZ-u.
Svaki član tima ravnopravno doprinosi razvoju i dokumentaciji.

# 📝 Kodeks ponašanja [![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md)

Kao studenti sigurno ste upoznati s minimumom prihvatljivog ponašanja definiran 
u Kodeksu ponašanja studenata. Očekujemo da ćete poštovati 
[etički kodeks IEEE-a](https://www.ieee.org/about/corporate/governance/p7-8.html).

### Funkcioniranje tima:
- Rad podijeljen prema kompetencijama članova
- Komunikacija putem Teams platforme
- Sporovi se rješavaju dogovorom prema IEEE etičkom kodeksu

# 📝 Licenca

[![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

Ovaj repozitorij sadrži otvoreni obrazovni sadržaj licenciran prema pravilima 
Creative Commons licencije koja omogućava preuzimanje i dijeljenje uz navođenje 
autora, nekomercijalnu upotrebu i dijeljenje pod istim uvjetima.

[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License HR][cc-by-nc-sa]

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: https://creativecommons.org/licenses/by-nc/4.0/deed.hr
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg
