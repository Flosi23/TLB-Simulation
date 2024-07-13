## Summe über eine verkettete Liste

Um herauszufinden, wie sich die Speicherzugriffe auf bei einer Summe über eine verkettete Liste verhalten, werden wir
folgendes C Programm betrachten:

```c
#include <stdio.h>

/* A Linked list node */
struct Node {
    int data;
    struct Node* next;
};

// function to find the sum of
// nodes of the given linked list
int sumOfNodes(struct Node* head)
{
    struct Node* ptr = head;
    int sum = 0;
    while (ptr != NULL) {

        sum += ptr->data;
        ptr = ptr->next;
    }

    return sum;
}

// Driver program to test above
int main()
{
    struct Node* head = NULL;
    sumOfNodes(head);
    return 0;
}
```

Kompilieren wir dieses Programm und betrachten das `.o` file:

<!--@formatter:off -->
```
main.o:	file format mach-o arm64

Disassembly of section __TEXT,__text:

0000000000000000 <ltmp0>:
       0: d10083ff     	sub	sp, sp, #32                     - Platz auf dem Stack reservieren                       
       4: f9000fe0     	str	x0, [sp, #24]                   - Argument 1 "head" wird auf dem Stack gespeichert
       8: f9400fe8     	ldr	x8, [sp, #24]                   - "head" wird in register x8 geladen - x8 entspricht der Variable "ptr"
       c: f9000be8     	str	x8, [sp, #16]                   - "ptr" wird auf dem Stack gespeichert
      10: b9000fff     	str	wzr, [sp, #12]                  - wzr = zero register wird auf dem Stack gespeichert - entspricht der Variable "sum" 
      14: 14000001     	b	0x18 <ltmp0+0x18>               - unbedingter Sprung zu 0x18
      18: f9400be8     	ldr	x8, [sp, #16]                   - "ptr" wird in Register x8 geladen
      1c: f1000108     	subs	x8, x8, #0                      - subs führt dazu, dass eq auf 1 gesetzt wird, wenn x8 = x8 - 0 anders geschrieben: x8 = 0 = NULL 
      20: 1a9f17e8     	cset	w8, eq                          - w8 wird auf 1 gesetzt wenn eq erfüllt = 1 ist
      24: 37000168     	tbnz	w8, #0, 0x50 <ltmp0+0x50>       - wenn w8 = 1 spring zu 0x50
      28: 14000001     	b	0x2c <ltmp0+0x2c>               - unbedingter Sprung zu 0x2c
      2c: f9400be8     	ldr	x8, [sp, #16]                   - "ptr" wird vom Stack in Register x8 geladen
      30: b9400109     	ldr	w9, [x8]                        - der Wert an Adresse "ptr" = "ptr->data" wird vom Heap in Register w9 geladen
      34: b9400fe8     	ldr	w8, [sp, #12]                   - "sum" wird in w8 geladen
      38: 0b090108     	add	w8, w8, w9                      - "ptr->data"  wird zu "sum" addiert
      3c: b9000fe8     	str	w8, [sp, #12]                   - "sum" wird auf dem Stack gespeichert
      40: f9400be8     	ldr	x8, [sp, #16]                   - "ptr" wird vom Stack in Register x8 geladen
      44: f9400508     	ldr	x8, [x8, #8]                    - der Wert and Adresse "ptr + 8" = ptr->next" wird vom Heap in Register x8 geladen
      48: f9000be8     	str	x8, [sp, #16]                   - "ptr" wird auf dem Stack gespeichert
      4c: 17fffff3     	b	0x18 <ltmp0+0x18>               - Unbedingter Sprung zu Schleifenanfang
      50: b9400fe0     	ldr	w0, [sp, #12]                   - "sum" wird ins Return register w0 geladen
      54: 910083ff     	add	sp, sp, #32                     - Stack wird freigegeben
      58: d65f03c0     	ret

000000000000005c <_main>:
      5c: d100c3ff     	sub	sp, sp, #48
      60: a9027bfd     	stp	x29, x30, [sp, #32]
      64: 910083fd     	add	x29, sp, #32
      68: 52800008     	mov	w8, #0
      6c: b9000fe8     	str	w8, [sp, #12]
      70: b81fc3bf     	stur	wzr, [x29, #-4]
      74: f9000bff     	str	xzr, [sp, #16]
      78: f9400be0     	ldr	x0, [sp, #16]
      7c: 94000000     	bl	0x7c <_main+0x20>
      80: b9400fe0     	ldr	w0, [sp, #12]
      84: a9427bfd     	ldp	x29, x30, [sp, #32]
      88: 9100c3ff     	add	sp, sp, #48
      8c: d65f03c0     	ret

Disassembly of section __LD,__compact_unwind:

0000000000000090 <ltmp1>:
		...
      98: 0000005c     	udf	#92
      9c: 02002000     	<unknown>
		...
      b0: 0000005c     	udf	#92
      b4: 00000000     	udf	#0
      b8: 00000034     	udf	#52
      bc: 04000000     	add	z0.b, p0/m, z0.b, z0.b
		...
```
<!--@formatter:on -->

Wir halten folgende Speicherzugriffe fest, wobei das Redundante laden von `ptr` in Register `x8` entfernt wurde, um
die Anzahl der Speicherzugriffe zu reduzieren und zu vereinfachen.

1. Initialisierung:
    1. `head` wird auf dem Stack `[sp, #24]` gespeichert
    2. `head` wird von `[sp, #24]` in Register `x8` geladen
    3. `ptr` wird auf dem Stack `[sp, #16]` gespeichert
    4. `zero` register wird auf dem Stack `[sp, #12]` gespeichert

2. Schleife:
    1. `ptr` wird von `[sp, #16]` in Register `x8` geladen
    2. `ptr->data` wird von Adresse `ptr` vom Heap in Register `w9` geladen
    3. `sum` wird von `[sp, #12]` in Register `w8` geladen
    4. `sum` wird auf `[sp, #12]` gespeichert
    5. `ptr->next` wird von Adresse `ptr+8` vom Heap in Register `x8` geladen
    6. `ptr->next` wird auf dem Stack `[sp, #16]` gespeichert

3. Schleifenende
    1. `sum` wird von `[sp, #12]` in Register `w0` geladen
