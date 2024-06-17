### Aufbau des TLB

#### Anforderungen

Gefordert wird ein direkt assoziativer TLB mit (ist nicht direkt gegeben) Cachezeilengröße 1.

#### Adressen

Wir gehen im folgenden davon aus, dass alle Adressen 32 Bit groß sind.
Eine virtuelle Adresse besteht aus 2 Teilen:

| Virtuele Seiten Nummer | Seiten Offset |
|------------------------|---------------|
| 32 - b Bits            | b Bits        |

Wobei `b = log_2(blocksize)`

Da sich die Blockgröße zwischen Seite und Kachel nicht verändert, ist eine physische Adresse gleich aufgebaut.

| Physische Kachel Nummer | Kachel Offset |
|-------------------------|---------------|
| 32 - b Bits             | b Bits        |

----

#### Cachezeilengröße

Die Cachezeilengröße des TLB ist wie folgt herzuleiten:
Man benötigt `log_2(tlbSize) = i` Bits um die Cachezeile zu adressieren.
Die virtuelle Adresse ist also:

| Tag Bits        | Index Bits | Seiten Offset |
|-----------------|------------|---------------|
| 32 - b - i Bits | i Bits     | b Bits        |

In einer Cachezeile müssen die Tag Bits der virtuellen Adressen und die zugehörige
physische Kachelnummern gespeichert werden.

Es werden also insgesamt:

| Tag Bits        | Kachel Nummer | 
|-----------------|---------------|
| 32 - b - i Bits | 32 - b  Bits  |

`cSize = 32 -b - i + 32 - b = 64 - 2b - i` Bits benötigt.

#### Implementierung

Es bietet sich an den TLB mit einem `array` der Länge `tlbSize` zu simulieren.

Da für die Simulation nicht relevant ist, wie die Daten im TLB gespeichert sind kann einfach ein `array` von `structs`
verwendet werden wordurch
man sich das komplizierte splitten des Wertes im TLB sparen kann.

Wir wissen außerdem dass sowohl tag Bits als auch Kachel Bits < 32 sind, also können wir die beiden Werte in
einem `uint32_t` speichern.

```c++
struct TLBEntry {
    uint32_t tag;
    uint32_t physicalFrameNumber;
};
```

### Ablauf eines Lookup im TLB

1. Virtuelle Adresse in Tag, Index und Offset aufteilen
2. Index in TLB nachschlagen
3. Wenn Eintrag gefunden:
    1. Tag vergleichen
    2. Wenn Tag übereinstimmt, dann fertig
6. Wenn Eintrag nicht gefunden:
    1. Virtuelle Adresse in physische Adresse umrechnen
    2. Eintrag in TLB schreiben
    9. Fertig

---

### Schritte einer Request durch die TLB Simulation

2. Wenn physische Adresse im TLB:
    1. Lesen der Virtuellen Adresse mit entsprechender Latenz `tlbLatency`
4. Wenn nicht physische Adresse im TLB:
    2. Virtuelle Adresse mit: `p = v + v2bBlockOffset * blocksize` berechnen und Latenz eines Hauptspeicherzugriffes
       `memoryLatency` simulieren
6. Lesen des Wertes an Adresse `v` aus dem Hauptspeicher mit Latenz `memoryLateny` 