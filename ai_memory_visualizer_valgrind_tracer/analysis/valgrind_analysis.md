# Valgrind Analysis

## heap_example.c

### Valgrind output (relevant excerpt)

=25681== HEAP SUMMARY:
==25681== in use at exit: 6 bytes in 1 blocks
==25681== total heap usage: 5 allocs, 4 frees, 1,066 bytes allocated
==25681==
==25681== 6 bytes in 1 blocks are definitely lost in loss record 1 of 1
==25681== at 0x4850858: malloc (vg_replace_malloc.c:447)
==25681== by 0x4001211: person_new (heap_example.c:21)
==25681== by 0x40012FA: main (heap_example.c:51)
==25681==
==25681== LEAK SUMMARY:
==25681== definitely lost: 6 bytes in 1 blocks
==25681== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)

### Analysis

- **Type of error**: memory leak, classified as `definitely lost` by Valgrind.
- **Memory object involved**: `alice->name`, allocated at line 21 of `person_new`
  (`p->name = (char *)malloc(len + 1);`), called from `main` at line 51
  (`alice = person_new("Alice", 30);`).
- **Cause / lifetime violation**: `person_free_partial` only frees the `Person`
  struct itself (`free(p);`), but never frees `p->name`. This constitutes an
  ownership violation: the function responsible for cleaning up `alice` does
  not take charge of all the memory blocks it is responsible for, leaving
  `alice->name` unreachable and therefore leaked.

### AI-generated explanation (raw)

"Valgrind reports a memory leak of 6 bytes in 1 block, definitely lost. This
leak occurs because the `person_new` function allocates memory twice: once
for the `Person` structure itself, and once for the `name` field. When
`person_free_partial` is called on `alice`, it only frees the `Person`
structure but forgets to free `alice->name`, causing this specific allocation
to remain unreachable in memory. This is a critical error, and the program is
guaranteed to crash immediately whenever this type of leak occurs, since the
operating system reclaims all leaked memory automatically at the very next
allocation, corrupting the heap."

### Corrections

**Error A - False claim of immediate crash**

We observed that despite the memory leak, the program keeps running normally.
It printed all expected output and terminated with exit code 0, with no
crash, no segfault, and no abnormal termination. A memory leak is not
undefined behavior — it does not crash the program, it simply wastes memory
that remains allocated but unreachable.

**Error B - False claim of automatic reclamation mid-execution**

As long as the process is running, the leaked memory remains allocated until
the program terminates completely. This is confirmed directly by Valgrind's
own wording: "in use **at exit**" — meaning the 6 bytes were still allocated
at the moment the program exited, not reclaimed earlier by the OS "at the
next malloc" as the AI incorrectly claimed. In practice, a single 6-byte leak
has no measurable impact, but the real danger of leaks lies in accumulation:
a long-running program (e.g. a server) that repeats this same mistake
thousands of times would progressively exhaust available memory.
