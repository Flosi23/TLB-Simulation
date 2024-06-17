### Aufbau des TLB

#### Anforderungen

Gefordert wird ein direkt assoziativer TLB mit (ist nicht direkt gegeben) Cachezeilengröße 1.

#### Implementierung

Es bietet sich an den TLB mit einem `array` der Länge `tlbSize` zu simulieren.

---

Berechnen der Cachezeile für eine Adresse `v`:

Gegeben sind:
- Cachezeilengröße: `cSize`
- Anzahl der Cachezeilen: `tlbSize`

Der Index `ì` einer virtuellen Adresse `v` wird dabei wie folgt berechnet:
Blocknummer `b = v / cSize`. Cachezeile = `b mod tlbSize`

Am berechneten Index `i` werden dann die `tag` Bits der Adresse `v` abgelegt. Dadurch kann einfach überprüft werden ob
es sich bei einem schon belegtem Index um einen Cache Hit oder Conflict Miss handelt. 

Bei einem Cache Hit können 

### Schritte einer Request durch die TLB Simulation

2. Wenn physische Adresse im TLB:
   3. Lesen der Virtuellen Adresse mit entsprechender Latenz `tlbLatency`
4. Wenn nicht physische Adresse im TLB:
   5. Virtuelle Adresse mit: `p = v + v2bBlockOffset * blocksize` berechnen und Latenz eines Hauptspeicherzugriffes 
      `memoryLatency` simulieren
6. Lesen des Wertes an Adresse `v` aus dem Hauptspeicher mit Latenz `memoryLateny` 