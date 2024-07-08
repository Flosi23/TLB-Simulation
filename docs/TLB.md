# Translation Lookaside Buffer

Moderne Mehrbenutzersysteme verwenden virtuellen Speicher, um unter anderem Programme voneinander zu isolieren. Dabei
werden (virtuelle) Speicheradressen von Anwendungen auf physische Speicher- adressen abgebildet. Damit diese Abstraktion
für das Programm transparent bleibt, muss das System bei jedem Speicherzugriff die virtuelle Adresse in eine physische
Adresse übersetzen. Die Übersetzungstabelle liegt jedoch selbst im Hauptspeicher und Zugriffe benötigen viele Zyklen.
Hier kommt der Translation Lookaside Buffer (TLB) zum Einsatz, ein spezieller Cache, der die Adressübersetzung
beschleunigt. Die TLB speichert die Ergebnisse früherer Adressübersetzungen, sodass häufig genutzte Speicheradressen
schneller zugänglich sind.

## Übliche Größe

Die Größe eines TLB hängt von der Architektur ab. So unterschieden Hersteller beispielsweise zwischen
L1 und L2 TLBs, wobei L2 TLBs deutlich größer sind [[3]](#3-apple-optimization-guide).
Hier finden sich einige Beispiele für Prozessoren und deren TLB Größen:

- M1 Generation: 128 / 160 Einträge [[3]](#3-apple-optimization-guide)
- A14 Bionic: 128 / 256 Einträge [[3]](#3-apple-optimization-guide)
- M2, M3 und A15 Bionic: 192 / 256 Einträge [[3]](#3-apple-optimization-guide)
-

## Addressübersetzung

Im Folgenden wird davon ausgegangen, dass jede Adresse 32 Bit groß ist.

Der TLB übersetzt virtuelle Adressen in physische Adressen. Dabei bestimmen die oberen
Bits der virtuellen Adresse (auch Seitennummer genannt) die oberen Bits der physischen Adresse (auch Kachelnummer
genannt). [[1]](#1-intel-64-developer-manual)

Die unteren Bits der virtuellen Adresse (offset) bestimmen die unteren Bits der physischen Adresse. Dabei ist
die Grenze zwischen unteren und oberen Bits durch die Blockgröße bestimmt. [[1]](#1-intel-64-developer-manual)

Eine virtuelle Adresse ist also wie folgt aufgebaut.

| Virtuele Seiten Nummer | Seiten Offset |
|------------------------|---------------|
| 32 - b Bits            | b Bits        |

Wobei `b = log_2(blocksize)`

Da sich die Blockgröße zwischen Seite und Kachel nicht verändert, ist eine physische Adresse gleich aufgebaut.

| Physische Kachel Nummer | Kachel Offset |
|-------------------------|---------------|
| 32 - b Bits             | b Bits        |

### TLB Caching

Wie bei jedem Cache wird auch im TLB die virtuelle Adresse in Tag, Index und Offset aufgeteilt.

| Tag Bits        | Index Bits | Seiten Offset |
|-----------------|------------|---------------|
| 32 - b - i Bits | i Bits     | b Bits        |

Wobei `i` abhängig von der Größe und Assoziativität des TLB ist. Im Falle eines
direkt abbildenden TLB ist `i = log_2(tlbSize)`.

Um festzustellen, ob eine Adresse im TLB liegt, müssen die Tag Bits der virtuellen Adresse mit den Tag Bits
der Cachezeile mit dem Index der virtuellen Adresse verglichen werden.

### Cachezeilen-Aufbau

Der Aufbau eines TLB Eintrags ist von der Architektur abhängig. So speichert etwa Intel auf IA-32 Architketuren in einem
TLB Eintrag
die folgenden Informationen [[1]](#1-intel-64-developer-manual):

- Physische Adresse der zugehörigen Kachelnummer
- Flags zur Zugriffskontrolle (z.B. R/W, U/S, XD Flags)

Generell kann ein TLB Eintrag aber reduziert werden auf die Tag Bits, die physische Adresse und etwaige
Flags [[2]](#2-reasoning-about-tlbs).
Eine Cachezeile (equivalent zu einem TLB Eintrag, da der TLB direkt assoziativ ist) hat in dieser Simulation den Aufbau:

| Tag Bits        | Kachel Nummer | Valid Bit |
|-----------------|---------------|-----------| 
| 32 - b - i Bits | 32 - b  Bits  | 1 Bit     | 

Die Größe einer Cachezeile `cSize` beträgt also `64 - 2b - i + 1` Bits.
Die Gesamtgröße des TLB beträgt also `tlbSize * cSize` Bits.

# Quellen

##### 1. Intel 64 Developer Manual

[Intel 64 and IA-32 Architectures Software Developer's Manual, Volume 3 (3A, 3B, 3C & 3D): System Programming Guide, 2016](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-system-programming-manual-325384.pdf),
Seiten 140 - 143

##### 2. Reasoning about TLBs

[Syeda Hira Taqdees, Gerwin Klein. Reasoning about Translation Lookaside Buffers. In EPiC Series In Computing, Volume 46,
2017](https://easychair.org/publications/open/gNH) Seiten 490 - 508

##### 3. Apple Optimization Guide

Apple Silicon CPU Optimization Guide 3.0, 2024, Seiten 118 - 119