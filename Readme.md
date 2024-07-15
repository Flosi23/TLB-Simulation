Was muss hier rein?

1. persönlichen Beitrag jedes Gruppenmitglieds
2. Ergebnisse der Simulation der Summe über eine linked List zusammenfassen
3. Ergebnisse der Recherche: übliche Größen, Architekturen für TLBs sowie Hauptspeicher und TLB Latenzen in modernen
   Prozessoren

# Simulation der Summe über eine verkettete Liste

**DISCLAIMER**: Eine detaillierte Analyse ist in [docs/sum.md](./docs/sum.md) zu finden.

### Speicherzugriffe

1. Initialisierung:
    1. pointer auf aktuellen Knoten wird vom Stack gelesen
    2. Summe wird auf dem Stack mit 0 initialisiert

2. Schleife:
    1. Adresse auf Knoten wird vom Stack gelesen
    2. Daten des Knoten werden vom Heap gelesen
    3. Summe wird vom Stack gelesen
    4. Summe + Daten des Knotens wird auf dem Stack gespeichert
    5. Pointer auf nächsten Knoten wird vom Heap gelesen
    6. Pointer auf nächsten Knoten wird auf den Stack geschrieben

### Simulation

Die Simulation wird mit einer verketten Liste von 1000 Knoten durchgeführt.
Diese sind einmal auf einem Heap der Größe
`256MiB`([./examples/linked_list_large_heap.csv](./examples/linked_list_large_heap.csv)) und einmal auf einem Heap der
Größe `64KiB` ([./examples/linked_list_small_heap.csv](./examples/linked_list_small_heap.csv)) verteilt.
Die TLB Größe wurde von 1 bis 32 Einträgen variiert.

### Ergebnisse

#### Großer Heap

![Cycles](./docs/assets/linked_list_large_heap_cycles.png)
![HitMiss](./docs/assets/linked_list_large_heap_hit_miss.png)

#### Kleiner Heap

![Cycles](./docs/assets/linked_list_small_heap_cycles.png)
![HitMiss](./docs/assets/linked_list_small_heap_hit_miss.png)

### Fazit

Die Miss Rate des TLBs und damit auch die Anzahl der benötigten Zyklen bei einer
Summe über eine verkettete List extrem von der räumlichen Lokalität der Speicherzugriffe abhängig. Liegen alle Adressen
nah beieinander oder in einer geringen Anzahl von Blöcken, so kann die Anzahl der
Misses auf ein Minimum reduziert
werden (siehe [kleiner heap](#kleiner-heap)). Liegen die Adressen jedoch alle in verschiedenen Speicherblöcken, tritt
bei jedem neuen
Knoten
ein neuer Conflict Miss auf (siehe [großer heap](#großer-heap)).

#### Speedup

Der Speedup ist stark von der räumlichen Lokalität der Speicherzugriffe abhängig. Trotzdem lassen
sich einige Aussagen treffen. Die Ausführungszeit ohne TLB würde bei `6006 * 2 * 60 = 720720` (immer 1 zusätzlicher
Speicherzugriff für die Adressübersetzung, bei 60 Zyklen Speicherlatenz) liegen.

Selbst bei schlechter räumlicher Lokalität (wie im [großen Heap Beispiel](#großer-heap)) kann durch das Cachen des
Stackpointers
ein Speedup von ca. `30%` erreicht werden. Bei guter räumlicher Lokalität (wie
im [kleiner Heap Beispiel](#kleiner-heap))
ist sogar ein Speedup von ca. `40%` möglich. Es ist also definitiv sinnvoll, einen TLB in Prozessoren zu verwenden.





