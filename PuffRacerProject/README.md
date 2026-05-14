# PuffRacer – Spirometar za kontrolu trkačeg auta

## Opis projekta

PuffRacer je ugradbeni sustav koji koristi intenzitet puhanja korisnika 
za upravljanje brzinom virtualnog trkačeg auta. Sustav mjeri jakost 
puhanja putem potenciometra (simulacija) ili mikrofona te proporcionalno 
kontrolira brzinu auta prikazanu LED indikatorima.

## Zamišljeni uređaj

Uređaj se sastoji od:
- **ESP32 mikrokontrolera** kao glavne upravljačke jedinice
- **Potenciometra** kao simulacije senzora puhanja
- **LED dioda** (zelena, žuta, crvena) kao indikatora brzine
- **OLED zaslona** (opcionalno) za prikaz trenutne brzine

## Funkcionalnosti

| Intenzitet puhanja | Stanje auta | LED indikator |
|--------------------|-------------|---------------|
| Nema puhanja       | Auto stoji  | Sve ugašeno   |
| Slabo puhanje      | Sporo vozi  | Zelena LED    |
| Srednje puhanje    | Srednja brzina | Žuta LED   |
| Jako puhanje       | Maksimalna brzina | Crvena LED |

## Tehnička implementacija

- Očitavanje vrijednosti potenciometra (ADC)
- Mapiranje vrijednosti na razine brzine (0-3)
- Upravljanje LED diodama prema trenutnoj brzini
- Serijski ispis trenutne brzine za praćenje

## Tim - BVB

- Toma Bošnjak
- Borna Šafar

## Platforma

ESP32 | Wokwi simulator | Arduino framework
