### Was ist das Ziel?

- Von der Relevanz des Themas überzeugen
- Korrektheit der Implementierung untermauern

### Was muss recherchiert werden

1. Heute Standards für TLBs
    1. übliche Größe
    2. übliche Architekturen
    3. TLB-Latenz
    4. Hauptspeicherzugrifflatenz

### Was muss untersucht werden

2. Speicherzugriffverhalten einer Summe über eine verkettete Liste untersuchen,
   dabei sollen `csv` Dateien erstellt werden die beispielhaft, dass Speicherzugriffs verhalten einer Summe beschreiben

### Interesting links

1. [TLB Blocksize (ARM specific)](https://developer.arm.com/documentation/ddi0406/b/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Translation-Lookaside-Buffers--TLBs-/TLB-matching?lang=en)
2. [UNRELIABLE - Stackoverlfow post about TLB management and initliazation](https://stackoverflow.com/questions/21885801/writing-the-translation-lookaside-buffer)
3. [UNRELIABLE - Stackoverflow post about what happens after a TLB Miss, contains a lot of interesting follow up links](https://stackoverflow.com/questions/32256250/what-happens-after-a-l2-tlb-miss)