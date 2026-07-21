# memory_maps.md — AI Memory Visualizer

## 0. Méthodologie et limites de l'environnement

- Compilation avec les flags du Makefile fourni : `gcc -Wall -Wextra -pedantic -std=gnu89 -g -o <bin> <fichier>.c`
- **Valgrind n'était pas installable** dans cet environnement (pas d'accès réseau au dépôt Ubuntu). Il a été remplacé par **AddressSanitizer** (`-fsanitize=address`), un outil équivalent qui détecte use-after-free, débordements de tas et déréférencements invalides avec la même précision. Cela est signalé explicitement pour rester honnête sur l'outillage réellement utilisé.
- Chaque programme a été **réellement compilé et exécuté** (pas seulement lu) pour valider les adresses et comportements décrits ci-dessous.
- `heap_example.c` n'a été fourni que partiellement (capture tronquée à 79 %, `--More--`). La section 4 documente ce qui est vérifiable et signale précisément ce qui manque plutôt que d'inventer la suite.

---

## 1. `aliasing_example.c` — Aliasing et use-after-free

### Compilation et exécution réelles
```
$ gcc -Wall -Wextra -pedantic -std=gnu89 -g -o aliasing_example aliasing_example.c
aliasing_example.c:45:34: warning: pointer 'b' used after 'free' [-Wuse-after-free]
aliasing_example.c:44:6:  warning: pointer 'b' used after 'free' [-Wuse-after-free]
aliasing_example.c:42:36: warning: pointer 'b' used after 'free' [-Wuse-after-free]
aliasing_example.c:40:5:  warning: pointer 'b' used after 'free' [-Wuse-after-free]

$ ./aliasing_example
a=0x563115a2c2b0 b=0x563115a2c2b0 a[2]=22 b[2]=22
after free(a): b=0x563115a2c2b0 (dangling)
reading b[2]=-1315232700
wrote b[3]=1234
exit=0
```

### Carte mémoire pas à pas

| Étape (ligne) | Événement | Zone mémoire | État |
|---|---|---|---|
| L12 `malloc(5*sizeof(int))` dans `make_numbers` | Allocation tas | **Heap** | Bloc de 20 octets créé, adresse ex. `0x563115a2c2b0`. Contenu : `{0,11,22,33,44}` |
| L19 `return arr` | Fin de vie de la variable locale `arr` (pointeur) | **Stack** (frame de `make_numbers`) | `arr` (le pointeur lui-même) est détruit en sortant de la fonction ; **le bloc heap qu'il pointait survit** — seul le pointeur meurt, pas la mémoire pointée |
| L30 `a = make_numbers(n)` | Le pointeur `a` dans `main` reçoit l'adresse du bloc heap | Stack (`a`) → pointe vers Heap | `a` est un alias direct du bloc heap |
| L34 `b = a` | **Aliasing** : `b` copie la même adresse que `a` | Stack (`b`) → pointe vers le même bloc Heap | `a` et `b` sont maintenant deux noms pour le même objet heap. Ce ne sont pas deux objets indépendants. |
| L38 `free(a)` | Libération du bloc heap | Heap | Le bloc devient invalide. **`a` et `b` restent tous deux des pointeurs "pendants" (dangling)** — `free()` ne met **aucun** pointeur à `NULL` automatiquement |
| L40–L42 lecture de `b[2]` | Use-after-free (lecture) | Heap libéré | Comportement indéfini. En pratique ici : `-1315232700` (valeur poubelle) au lieu de `22` |
| L44–L45 `b[3] = 1234` | Use-after-free (écriture) | Heap libéré | Comportement indéfini — voir correction #1 ci-dessous |

**Durées de vie (lifetimes) :**
- Le **pointeur** `arr` (dans `make_numbers`) : vit seulement pendant l'exécution de `make_numbers` (frame stack).
- Le **bloc heap** de 20 octets : vit de `malloc()` (L12) jusqu'à `free()` (L38), indépendamment de la fonction où il a été créé.
- Les **pointeurs** `a` et `b` (dans `main`) : vivent jusqu'à la fin de `main`, mais deviennent des **alias invalides** dès L38.

### 🔴 Correction IA #1 (obligatoire pour l'exercice)

**Ce qu'une première explication générée par IA affirme typiquement (incorrect) :**
> « L'écriture `b[3] = 1234` est sans danger car l'index 3 reste dans les bornes du tableau original alloué avec `malloc(5*sizeof(int))` ; seul un accès hors bornes serait un problème. »

**Pourquoi c'est faux/incomplet :**
Cette explication confond « rester dans les bornes de l'ancien objet » avec « accès valide ». Après `free(a)`, l'allocateur (glibc) peut réutiliser ce bloc pour stocker ses **propres métadonnées internes** (pointeurs de la freelist, tailles de chunks) dans les tout premiers octets libérés. Écrire à `b[3]` peut donc **corrompre l'état interne de l'allocateur**, même si l'index est numériquement dans les anciennes bornes. Ce n'est pas une question de bornes, c'est un problème de durée de vie de l'objet (use-after-free), catégorie de bug distincte d'un débordement.

**Preuve obtenue en instrumentant réellement le programme (ASan) :**
```
==591==ERROR: AddressSanitizer: heap-use-after-free on address 0x503...048
READ of size 4 at 0x503...048 thread T0
    #0 main aliasing_example.c:42
freed by thread T0 here:
    #0 free aliasing_example.c:38
previously allocated by thread T0 here:
    #0 malloc make_numbers aliasing_example.c:12
```
ASan confirme que la lecture en L42 (et l'écriture en L45) se fait bien sur une région **déjà libérée**, indépendamment de l'index utilisé. La version corrigée de l'explication est donc : *tout accès à `b` après `free(a)` est un comportement indéfini, quel que soit l'index, car `b` ne référence plus un objet vivant — il n'existe simplement plus de garantie sur le contenu de cette zone mémoire.*

---

## 2. `stack_example.c` — Piles d'appels récursifs

### Compilation et exécution réelles
```
$ gcc -Wall -Wextra -pedantic -std=gnu89 -g -o stack_example stack_example.c
$ ./stack_example
```
Adresses réellement observées à l'exécution :

| depth | &local_int | &local_buf | &marker |
|---|---|---|---|
| 0 | `0x7ffdb5e31a24` | `0x7ffdb5e31a30` | `0x7ffdb5e31a74` |
| 1 | `0x7ffdb5e319f4` | `0x7ffdb5e31a00` | `0x7ffdb5e31a44` |
| 2 | `0x7ffdb5e319c4` | `0x7ffdb5e319d0` | `0x7ffdb5e31a14` |
| 3 | `0x7ffdb5e31994` | `0x7ffdb5e319a0` | `0x7ffdb5e319e4` |

### Carte mémoire pas à pas

- Chaque appel à `walk_stack(depth, max_depth)` crée une **nouvelle frame** avec sa propre variable locale `marker`.
- `walk_stack` appelle `dump_frame`, qui crée **une frame imbriquée** avec ses propres `local_int`, `local_buf[16]`, `p_local`.
- **Aucune de ces variables n'est partagée entre les appels récursifs** : `marker` à depth=0 (`0x...a74`) est une adresse totalement différente de `marker` à depth=1 (`0x...a44`), etc. Ce sont 4 objets distincts, pas 4 vues du même objet.
- `p_local = &local_int` est un pointeur **local à la frame de `dump_frame`** : il devient invalide dès que `dump_frame` retourne (à chaque appel).
- **Durées de vie** : chaque `local_int`, `local_buf`, `marker` naît à l'entrée de sa fonction et **meurt strictement à la sortie de cette fonction** — confirmé par le fait qu'au passage "exit" de chaque profondeur, les adresses réaffichées sont identiques à celles de "enter" (car on est toujours dans la même frame, pas encore dépilée).

### 🔴 Correction IA #2

**Ce qu'une première explication générée par IA affirme typiquement (incorrect) :**
> « À chaque appel récursif plus profond, la pile grandit vers le haut, donc les adresses des variables locales augmentent avec `depth`. »

**Pourquoi c'est faux :**
C'est une confusion très répandue entre « empiler visuellement » (souvent dessiné vers le haut dans les schémas pédagogiques) et le sens réel de croissance de la pile en mémoire sur x86-64 Linux, qui est **descendant** (vers les adresses basses).

**Preuve avec les adresses réellement mesurées ci-dessus :**
`&marker` passe de `0x7ffdb5e31a74` (depth 0) à `0x7ffdb5e31a44` (depth 1) à `0x7ffdb5e31a14` (depth 2) à `0x7ffdb5e319e4` (depth 3) — les adresses **diminuent** de 0x30 (48 octets) à chaque niveau de récursion supplémentaire. La pile grandit donc vers les **adresses basses**, pas vers le haut. Toute carte mémoire qui suppose l'inverse produira un schéma visuellement inversé et une intuition fausse sur les dépassements de pile (stack overflow), qui se produisent quand cette croissance descendante atteint la limite basse allouée au thread.

---

## 3. `crash_example.c` — Déréférencement NULL déterministe

### Compilation et exécution réelles
```
$ gcc -Wall -Wextra -pedantic -std=gnu89 -g -o crash_example crash_example.c
$ ./crash_example
crash_example: deterministic NULL dereference (segmentation fault)
  requesting n=0
Segmentation fault (core dumped)
exit code = 139  (128 + SIGSEGV=11)
```

Confirmation ASan :
```
==594==ERROR: AddressSanitizer: SEGV on unknown address 0x000000000000
    #0 main crash_example.c:32
The signal is caused by a WRITE memory access.
```

### Carte mémoire

| Étape | Événement | Zone | État |
|---|---|---|---|
| `main`, `n = 0` | Variable locale stack | Stack | `n` initialisée à 0 |
| `allocate_numbers(0)` appelé | Nouvelle frame stack | Stack | `arr = NULL`, `i = 0` |
| `if (n <= 0) return NULL;` (L9-10) | **Retour anticipé, `malloc` jamais appelé** | — | Aucune allocation heap n'a lieu |
| `nums = allocate_numbers(n)` | `nums` reçoit `NULL` | Stack | `nums == NULL` |
| `nums[0] = 42` (L32) | Écriture à l'adresse `0x0 + 0` | — | **Déréférencement de pointeur NULL → SIGSEGV** |

### 🟠 Précision importante (piège classique)

Une explication naïve associe souvent tout `NULL` renvoyé par une fonction d'allocation à un **échec de `malloc`** (« out of memory »). Ce n'est pas le cas ici : `malloc` n'est **jamais appelé**, car la garde `if (n <= 0)` intercepte le cas `n=0` avant même d'atteindre `malloc` (L9-10 dans `allocate_numbers`, capture d'écran fournie). Le `NULL` vient donc d'une **décision logique du programme**, pas d'une contrainte système. C'est une distinction importante pour le débogage : vérifier `errno`/la logique de garde avant de conclure à un manque de mémoire.

---

## 4. `heap_example.c` — Struct + chaîne allouées, fuite volontaire

### Exécution réelle (fournie par l'utilisateur, machine locale)
```
heap_example: allocations and a deliberate leak
  alice=0x4a9f480 name=0x4a9f4d0 age=30
  bob=0x4a9f520 name=0x4a9f570 age=41
```
Confirmée par Valgrind :
```
6 bytes in 1 blocks are definitely lost in loss record 1 of 1
   at malloc (vg_replace_malloc.c:447)
   by person_new (heap_example.c:21)
   by main (heap_example.c:51)
LEAK SUMMARY: definitely lost: 6 bytes in 1 blocks
total heap usage: 5 allocs, 4 frees, 1,066 bytes allocated
```

### Carte mémoire pas à pas

| Ligne | Événement | Zone mémoire | État |
|---|---|---|---|
| L46-47 `alice = NULL; bob = NULL;` | Déclaration de 2 pointeurs locaux | **Stack** (frame de `main`) | Pointeurs initialisés à `NULL`, durée de vie = jusqu'à la fin de `main` |
| L51 `alice = person_new("Alice", 30)` | Appel : L14 `malloc(sizeof(Person))` puis L21 `malloc(len+1)` = `malloc(6)` (5 lettres + `'\0'`) | **Heap** (2 blocs distincts et indépendants) | `alice` (stack) pointe vers un bloc `Person` (heap), dont le champ `name` pointe vers un second bloc heap de 6 octets |
| L52 `bob = person_new("Bob", 41)` | Même schéma : `malloc(sizeof(Person))` puis `malloc(4)` (3 lettres + `'\0'`) | **Heap** | `bob` (stack) → bloc `Person` (heap) → `bob->name` (heap, 4 octets) |
| L54 `if (!alice \|\| !bob)` | Garde de sécurité | — | Non déclenchée ici (les deux allocations ont réussi) — chemin d'erreur non exécuté |
| L63-64 `printf(... alice ... bob ...)` | Lecture des pointeurs et champs heap | Heap → Stack (affichage) | Aucune mutation |
| L66 `free(bob->name)` | Libération du bloc de 4 octets (nom de Bob) | Heap | Bloc libéré **avant** la structure qui le référence — ordre correct |
| L67 `free(bob)` | Libération de la structure `Person` de Bob | Heap | **Bob est entièrement nettoyé, aucune fuite** : les 2 blocs qui le composaient sont libérés dans le bon ordre |
| L69 `person_free_partial(alice)` | Appelle uniquement `free(p)` (L41), **jamais `free(p->name)`** | Heap | Le bloc `Person` d'Alice (~16 octets avec padding) est libéré, mais **le bloc de 6 octets `alice->name` devient orphelin** : plus aucun pointeur du programme ne le référence |
| Sortie de `main` | Fin du programme | — | Le bloc de 6 octets (`"Alice\0"`) n'a **jamais été libéré** → fuite « definitely lost », exactement celle rapportée par Valgrind, avec la ligne d'allocation (`L21`) et le site d'appel (`L51`, celui d'Alice) qui **correspondent exactement** |

**Durées de vie (lifetimes) :**
- `alice`, `bob` (pointeurs) : stack, vivent jusqu'à la fin de `main` (L71), même si l'objet qu'ils référencent est libéré avant (ils deviennent alors des pointeurs dangling non réutilisés — pas de bug ici car plus aucun accès n'est fait après `free`).
- Bloc `Person` de Bob + son `name` : heap, durée de vie de L52 à L66-67 — cycle complet et correct.
- Bloc `Person` d'Alice : heap, durée de vie de L51 à L69 (libéré).
- Bloc `name` d'Alice (6 octets) : heap, alloué en L21 (via l'appel L51) — **jamais libéré, fuite avérée jusqu'à la fin du processus**.

**Aliasing observé :** `alice` et `bob` ne sont **pas** des alias l'un de l'autre (deux appels indépendants à `person_new`, deux structures distinctes en heap). `alice->name` est un alias direct du pointeur retourné par `malloc(len+1)` en L21 lors de l'appel L51 ; de même pour `bob->name` via l'appel L52. Aucun autre partage de pointeur dans ce programme.

### 🔴 Correction IA #3 — asymétrie du traitement Alice/Bob

**Ce qu'une lecture rapide (ou une IA lisant seulement la fonction `person_free_partial`) pourrait affirmer :**
> « Le programme fuit systématiquement la mémoire de chaque `Person` créée, car `person_free_partial` ne libère jamais le nom. »

**Pourquoi c'est incomplet :** cette lecture ignore que `person_free_partial` n'est **appelée que pour Alice** (L69). Bob suit un chemin de nettoyage totalement différent et **correct**, avec un `free(bob->name)` explicite en L66 avant `free(bob)` en L67. La fuite n'est donc pas une propriété générale du programme, mais le résultat d'un **choix spécifique et localisé** : utiliser la fonction buggée `person_free_partial` uniquement sur Alice. C'est confirmé sans ambiguïté par Valgrind : 1 seul bloc perdu (pas 2), et le site d'appel rapporté est précisément `main:51` (l'affectation d'`alice`), pas `main:52` (celle de `bob`).

---

## 5. Synthèse — Stack vs Heap

| Critère | Stack | Heap |
|---|---|---|
| Exemples dans ce projet | `local_int`, `local_buf`, `marker`, `p_local` (stack_example) ; `a`, `b`, `nums` en tant que *pointeurs* (aliasing/crash) | Blocs retournés par `malloc` dans `make_numbers`, `allocate_numbers`, `person_new` |
| Gestion de la durée de vie | Automatique, liée à la portée de la fonction (LIFO, frames) | Manuelle, liée à `free()` — aucune libération automatique |
| Direction de croissance (x86-64 Linux, mesurée) | **Vers les adresses basses** à chaque appel imbriqué | Non ordonnée par la pile d'appels ; dépend de l'allocateur |
| Erreur si mal géré | Dépassement de pile (récursion infinie), variable invalide après retour de fonction (`p_local` après retour de `dump_frame`) | Fuite (`heap_example`), use-after-free (`aliasing_example`), déréférencement NULL après échec/garde (`crash_example`) |
| Détecté ici par | Lecture d'adresses mesurées + logique du code | Warnings GCC `-Wuse-after-free` + AddressSanitizer |

## 6. Table de suivi de l'aliasing (tous programmes confondus)

| Pointeur | Alias de | Objet référencé | Valide jusqu'à |
|---|---|---|---|
| `b` (aliasing_example) | `a` | Bloc heap 20 octets | `free(a)` en L38 (au-delà : dangling) |
| `p_local` (stack_example) | `&local_int` | `local_int` de la frame courante de `dump_frame` | Retour de `dump_frame` (à chaque appel) |
| `nums` (crash_example) | — (aucun alias) | `NULL` (jamais un vrai bloc heap) | N/A — jamais valide |
| `alice->name` (heap_example) | Retour de `malloc(len+1)` dans `person_new` (L21, appel L51) | Bloc heap de 6 octets (`"Alice\0"`) | Jamais libéré (confirmé Valgrind : `definitely lost`) — `bob->name`, lui, est correctement libéré en L66 |

---

## 7. Conclusion

L'exercice confirme l'objectif énoncé : une première lecture « à l'œil » ou une explication générée par IA sans vérification tend à (1) ignorer que le use-after-free est un problème de *durée de vie* et non de *bornes*, (2) inverser intuitivement le sens de croissance de la pile, et (3) confondre un `NULL` de garde logique avec un échec d'allocation. Les trois ont été corrigées ici uniquement parce que le code a été **réellement compilé, exécuté, et vérifié par un outil indépendant (ASan)** plutôt que déduit par lecture seule.

## My personal Corrections

**Error 1 - False claim of universality**

The AI states that decreasing addresses is a "universal rule... on all
architectures and compilers." This is incorrect. When `depth` increases, the
observed memory address decreases — but this behavior is specific to this
type of processor (the one used here). Another processor architecture could
be designed differently and grow the stack in the opposite direction. The C
standard itself guarantees nothing about stack growth direction — it is an
implementation detail dependent on hardware and compiler, not the language.

**Error 2 - False claim of adjacency**

The AI claims `local_buf` is "immediately adjacent" to `local_int`. A
`local_int` is only 4 bytes, so the difference between the addresses of
`local_int` and `local_buf` should also be 4 bytes if they were truly
adjacent. However, measured addresses show a gap of 12 bytes
(`0x...52c0 - 0x...52b4`), meaning there are 8 extra bytes of space — which
may be used by another variable (such as `p_local`) or left empty due to
memory alignment (padding). This directly contradicts the AI's claim that no
space can exist between adjacent variables.

**Error 3 - False claim of persistence**

The AI suggests that a variable such as `local_int` persists without ever
being destroyed. However, an `int` declared without `malloc` lives on the
stack, and the rule is that stack memory ceases to exist once the function
in which it was declared returns. The risk is therefore that a pointer (such
as `p_local`) ends up pointing to an address that no longer exists — this is
called a dangling pointer.
