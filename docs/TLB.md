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
L1 und L2 TLBs, wobei L2 TLBs deutlich größer sind. Außerdem hängt die TLB Größe von der Page Size, für die der TLB
ausgelegt ist ab.
Hier finden sich einige Beispiele für Prozessoren / Architekturen und deren TLB Größen:

- M1 Generation: L1 TLB: 128 / 160 Einträge [[3]](#3-apple-optimization-guide)
- A14 Bionic: L1 TLB: 128 / 256 Einträge [[3]](#3-apple-optimization-guide)
- M2, M3 und A15 Bionic: L1 TLB: 192 / 256 Einträge [[3]](#3-apple-optimization-guide)
- AMD Zen (17h Familie): L1 TLB: 64 Einträge, L2 TLB: 1536 / 1024 Einträge [[4]](#4-amd-zen-reference)
- Intel Core Microarchitecture: DTLB0: 16 Einträge, DTLB1: 32 / 256 Einträge [[5]](#5-intel-64-optimization-manual)
- Intel Sandy Bridge Microarchitecture: DTLB: 4 / 64 Einträge, STLB: 512
  Einträge [[5]](#5-intel-64-optimization-manual)
- Intel Skylake Microarchitecture: STLB: 128 Einträge, DTLB1: 4 / 64, DTLB2: 16 / 1536
  Einträge [[5]](#5-intel-64-optimization-manual)

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

### Caching

Bei Caching wird die virtuelle Adresse normalerweise in Tag, Index und Offset aufgeteilt.

| Tag Bits        | Index Bits | Seiten Offset |
|-----------------|------------|---------------|
| 32 - b - i Bits | i Bits     | b Bits        |

Wobei `i` abhängig von der Größe und Assoziativität des Caches ist. Im Falle eines
direkt abbildenden TLB ist `i = log_2(cacheSize)`.

Um festzustellen, ob eine Adresse im Cache liegt, müssen die Tag Bits der virtuellen Adresse mit den Tag Bits
der Cachezeile mit dem Index der virtuellen Adresse verglichen werden.

#### Handling von nicht Zweierpotenzen als TLB Größen

Wenn die Größe eines TLB keine Zweierpotenz ist, kann es passieren, dass einer Adresse kein valider Index zugeordnet
werden kann.
Man nehme z.B. einen TLB der Größe 39 und Blocksize = 4096. Dann ist die Anzahl der Index
Bits `i = ceil(log_2(39)) = 6`. und die Anzahl der
Offset Bits `b = log_2(4096) = 12`. Betrachte man nun die Adresse `0x92b3900`, welcher der Index `110011 = 51`
zugewiesen wird. Da der TLB jedoch nur 39
Einträge hat, ist der Index nicht valide.

Hersteller vermeiden dieses Problem oft ganz, indem sie die TLB Größe auf eine Zweierpotenz setzen oder aber die
Cache-Assoziativität so wählen, dass die Anzahl der Cache-Sets eine Zweierpotenz ist und somit die Bestimmung des
Indexes unproblematisch ist.
Ein Beispiel hierfür ist die AMD Zen Prozessor Familie, welche eine L2 DTLB Größe von 1536 Einträgen hat, und 6-Fach
assoziativ ist. [[4]](#4-amd-zen-reference)
Es ergeben sich also: `1536 / 6 = 256` Sets.

In dieser Simulation ist das Problem nicht so einfach zu umgehen, da der TLB direkt abbildend ist und die Größe des
TLB
flexibel gewählt werden kann. Eine mögliche Lösung ist es die virtuelle Seitennummer modulo der TLB Größe zu nehmen um
so  
den Index zu bestimmen. Im TLB Eintrag wird dann die gesamte virtuelle Seitennummer als Tag gespeichert, da nur die Tag
Bits
nun nicht mehr ausreichen, um eine Adresse eindeutig zu identifizieren.

### TLB-Eintrag-Aufbau

Der Aufbau eines TLB Eintrags ist von der Architektur abhängig. So speichert etwa Intel auf IA-32 Architketuren in einem
TLB Eintrag
die folgenden Informationen: [[1]](#1-intel-64-developer-manual)

- Physische Adresse der zugehörigen Kachelnummer
- Flags zur Zugriffskontrolle (z.B. R/W, U/S, XD Flags)

Generell kann ein TLB Eintrag aber reduziert werden auf die Tag Bits, die physische Adresse und etwaige
Flags. [[2]](#2-reasoning-about-tlbs)
Ein Eintrag im TLB speichert in dieser Simulation zusätzlich zu den Tag Bits der Virtuellen Adresse auch noch die Index
Bits (also die ganze virtuelle Seitennummer) um mit TLB Größen
die keine Zweierpotenz sind umgehen zu können (
siehe: [Handling von nicht Zweierpotenzen als TLB Größen](#handling-von-nicht-zweierpotenzen-als-tlb-größen)). Der
Aufbau ist also wie folgt:

| Virtuelle Seitennummer | Kachel Nummer | Valid Bit |
|------------------------|---------------|-----------| 
| 32 - b Bits            | 32 - b  Bits  | 1 Bit     | 

Die Größe einer Cachezeile `cSize` beträgt also `64 - 2b + 1` Bits.
Die Gesamtgröße des TLB beträgt also `tlbSize * cSize` Bits.

# Quellen

##### 1. Intel 64 Developer Manual

[Intel 64 and IA-32 Architectures Software Developer's Manual, Volume 3 (3A, 3B, 3C & 3D): System Programming Guide, 2016](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-system-programming-manual-325384.pdf),
Seiten 140 - 143

##### 2. Reasoning about TLBs

[Syeda Hira Taqdees, Gerwin Klein. Reasoning about Translation Lookaside Buffers. In EPiC Series In Computing, Volume 46,
2017](https://easychair.org/publications/open/gNH) Seiten 490 - 508

##### 3. Apple Optimization Guide

Apple Silicon CPU Optimization Guide 3.0, 2024, Seiten 117 - 119

#### 4. AMD Zen Reference

[Processor Programming Reference (PPR) for AMD Family 17h Model 01h, Revision B1 Processors, 2017](https://bugzilla.kernel.org/attachment.cgi?id=287389)
Seiten 68 - 70

#### 5. Intel 64 Optimization Manual

[Intel 64 and IA-32 Architectures Optimization Reference Manual, 2019](https://en.wikichip.org/w/images/8/8a/intel-ref-248966-042b.pdf)
Seiten 56, 79, 92

Interresante Seiten: 97, 745 (memory Latency)