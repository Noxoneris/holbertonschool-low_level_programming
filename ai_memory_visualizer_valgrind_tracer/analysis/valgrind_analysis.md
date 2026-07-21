# valgrind_analysis.md — Valgrind & AI Memory Tracer

## 0. Méthodologie

Analyse basée exclusivement sur les **sorties Valgrind réellement produites par l'utilisateur** (Valgrind 3.26.0, options `--leak-check=full --show-leak-kinds=all --track-origins=yes`) sur les 4 binaires : `stack_example`, `heap_example`, `aliasing_example`, `crash_example`. Aucune compilation n'a été refaite ici ; chaque avertissement est confronté au code source (Task 1) et au `memory_maps.md` déjà produit.

---

## 1. `stack_example` — 0 erreur

```
HEAP SUMMARY: in use at exit: 0 bytes in 0 blocks
total heap usage: 1 allocs, 1 frees, 1,024 bytes allocated
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
```

**Classification :** Aucune. Le seul `alloc`/`free` (1 024 octets) n'appartient pas au code utilisateur (aucun `malloc` n'apparaît dans `stack_example.c`) — c'est un **buffer interne de la libc** (tampon de `stdio` pour `printf`), alloué et libéré automatiquement autour de `main`.

**Point important à comprendre, pas seulement à constater :** un "0 errors" de Valgrind ne valide **que la mémoire heap suivie par Memcheck**. Il ne dit strictement rien sur la correction de l'usage de la pile (adresses de `local_int`, `local_buf`, `marker` observées et déjà validées dans `memory_maps.md`). Memcheck ne track pas nativement les dépassements de pile ni les variables locales retournées par pointeur (il faudrait `--tool=exp-sgcheck` ou ASan `-fsanitize=address` pour ça). Ici, l'absence d'erreur est donc **cohérente avec** un usage correct de la pile, mais ne la **prouve pas** à elle seule.

---

## 2. `heap_example` — 1 fuite « definitely lost »

```
in use at exit: 6 bytes in 1 blocks
total heap usage: 5 allocs, 4 frees, 1,066 bytes allocated
6 bytes in 1 blocks are definitely lost in loss record 1 of 1
   at malloc (vg_replace_malloc.c:447)
   by person_new (heap_example.c:21)
   by main (heap_example.c:51)
LEAK SUMMARY: definitely lost: 6 bytes in 1 blocks
```

**Classification précise :** fuite mémoire de type **« definitely lost »** (terminologie Valgrind) — la plus grave des 4 catégories, car elle signifie qu'**aucune chaîne de pointeurs vivante ne mène plus à ce bloc** à la fin du programme. À distinguer de :
- *indirectly lost* : bloc perdu uniquement parce qu'un autre bloc pointant vers lui est lui-même *definitely lost* (non applicable ici, il n'y a qu'un seul record) ;
- *possibly lost* : un pointeur existe encore mais pointe à l'intérieur du bloc, pas à son origine (non applicable ici) ;
- *still reachable* : un pointeur valide existe toujours à la sortie mais n'a jamais été libéré (non applicable ici, voir §4 pour un vrai cas).

**Objet mémoire concerné, identifié précisément :** un bloc de **6 octets**, alloué par `malloc` en `heap_example.c:21` (à l'intérieur de `person_new`, sur la ligne `p->name = (char *)malloc(len + 1)`), retourné vers un appel de `person_new` fait en `main:51`.

La taille (6 octets) permet une identification non ambiguë de l'objet, indépendamment du numéro de ligne exact du fichier complet que je n'ai que partiellement : **`"Alice"` fait 5 caractères + 1 octet `'\0'` = 6 octets**, alors que `"Bob"` ferait 3 + 1 = 4 octets. Le bloc perdu est donc **précisément le champ `name` de la structure `Person` créée pour Alice**, pas celle de Bob.

**Cause exacte de la fuite (violation de durée de vie / perte d'ownership) :** `person_free_partial(p)` (visible dans le code fourni) exécute uniquement `free(p)` — il libère la structure `Person`, **mais jamais `p->name`**. Or `p->name` était le **seul pointeur vivant** vers ce bloc de 6 octets (aucun autre alias n'existe dans le programme). En détruisant la structure qui le contenait sans l'avoir d'abord libéré, le programme perd **toute possibilité future** d'appeler `free()` dessus : c'est une perte d'ownership classique, pas un oubli de portée.

**Confirmation par le code source complet (`main`, L44-71) :** 5 allocations au total = 2×`malloc(sizeof(Person))` (Alice L51, Bob L52) + 2×`malloc(len+1)` (les deux noms) + 1 allocation interne libc (buffer stdio, comme en §1). Le code confirme exactement l'hypothèse déduite des seuls compteurs Valgrind : **Bob est nettoyé correctement** via `free(bob->name); free(bob);` (L66-67, soit 2 frees), tandis qu'**Alice passe par `person_free_partial(alice)`** (L69), qui n'exécute que `free(p)` — d'où la fuite unique de 6 octets. Le site d'appel rapporté par Valgrind (`main:51`, l'affectation d'`alice`) correspond exactement à la ligne réelle, ce qui valide l'identification.

---

## 3. `aliasing_example` — 3 erreurs, 0 fuite

```
ERROR SUMMARY: 3 errors from 3 contexts
HEAP SUMMARY: in use at exit: 0 bytes in 0 blocks
All heap blocks were freed -- no leaks are possible
```

Trois avertissements, tous de la même famille mais à des lignes différentes :

| # | Type précis | Ligne | Détail |
|---|---|---|---|
| 1 | **Invalid read of size 4** | `aliasing_example.c:42` (`reading b[2]=%d`) | Adresse 8 octets à l'intérieur d'un bloc de 20 octets **déjà libéré** — offset 8 = `b[2]` (2×4 octets), donc c'est bien l'élément `b[2]` qui est lu après coup |
| 2 | **Invalid write of size 4** | `aliasing_example.c:44` (`b[3] = 1234;`) | Adresse 12 octets dans le même bloc libéré — offset 12 = `b[3]` (3×4 octets) |
| 3 | **Invalid read of size 4** | `aliasing_example.c:45` (`wrote b[3]=%d`) | Même bloc, même offset 12 — relecture de la valeur qu'on vient d'écrire |

Pour les 3 : `Block was alloc'd at malloc (make_numbers, aliasing_example.c:12)` puis `freed by free (main, aliasing_example.c:38)`.

**Classification exacte :** **use-after-free** (et non un débordement de tampon / *heap-buffer-overflow*). La distinction est cruciale et se lit dans le message Valgrind lui-même : `"Address ... is N bytes inside a block of size 20 free'd"` — l'accès reste **dans les bornes originales** de l'allocation (max offset 12 < 20), mais le bloc **n'existe plus en tant qu'objet valide** depuis `free(a)` en ligne 38. Un vrai débordement afficherait plutôt un message du type *"N bytes after a block of size M alloc'd"*.

**Pourquoi 0 fuite malgré 3 erreurs :** ces deux notions sont **orthogonales** dans le modèle de Valgrind. Une fuite (*leak*) concerne un bloc **jamais libéré**. Ici, le bloc de 20 octets **a bien été libéré** (`free(a)` en L38) ; le problème est qu'il a continué à être **utilisé après** cette libération. D'où `HEAP SUMMARY: All heap blocks were freed -- no leaks are possible` **en même temps que** 3 erreurs actives.

**Détail révélateur (déjà visible dans la sortie utilisateur) :** `reading b[2]=22` — la valeur relue après `free()` est **identique** à la valeur écrite avant la libération (`a[2]=22`), alors que dans le rapport `memory_maps.md` (obtenu via ASan sur cette machine sandbox) la même lecture donnait une valeur poubelle (`-1315232700`). C'est cohérent avec un comportement indéfini : `free()` ne garantit **aucune** valeur particulière en mémoire — le contenu peut rester intact par coïncidence (pas de réécriture immédiate par l'allocateur) ou être écrasé, selon l'implémentation et le contexte. C'est justement ce qui rend le use-after-free dangereux : il peut « sembler marcher » un grand nombre de fois avant de casser silencieusement.

---

## 4. `crash_example` — 1 erreur (pas une fuite), puis SIGSEGV

```
Invalid write of size 4
   at main (crash_example.c:32)
 Address 0x0 is not stack'd, malloc'd or (recently) free'd
Process terminating with default action of signal 11 (SIGSEGV)
...
HEAP SUMMARY: in use at exit: 1,024 bytes in 1 blocks
total heap usage: 1 allocs, 0 frees, 1,024 bytes allocated
LEAK SUMMARY: still reachable: 1,024 bytes in 1 blocks
ERROR SUMMARY: 1 errors from 1 contexts
```

**Classification exacte :** **déréférencement de pointeur NULL** (catégorie distincte du use-after-free). Le message `"Address 0x0 is not stack'd, malloc'd or (recently) free'd"` est la signature typique d'un accès à une adresse qui **n'a jamais appartenu à un objet quelconque** — ni pile, ni tas, ni bloc libéré récemment — contrairement au cas §3 où l'adresse *appartenait* à un bloc (juste libéré). Ici, c'est cohérent avec `memory_maps.md` : `nums = allocate_numbers(0)` renvoie `NULL` via la garde `if (n <= 0) return NULL;` **avant même d'appeler `malloc`**, donc `nums[0] = 42` (ligne 32) écrit à l'adresse `0x0`.

**Le bloc « still reachable » de 1 024 octets n'est PAS l'erreur signalée, et ce n'est pas une fuite :** c'est le même buffer interne `stdio` que dans les sections 1 et 2. Il apparaît ici comme *still reachable* (et non *leaked*) parce que le programme a été **interrompu par le SIGSEGV avant sa libération normale en fin de `main`** — mais un pointeur valide vers ce buffer existait encore au moment du crash (typiquement une variable statique interne à la libc), donc Valgrind le classe différemment d'un « definitely lost ». **Par défaut, les blocs *still reachable* ne comptent pas dans `ERROR SUMMARY`** — le seul et unique des 1 `errors` reporté est l'écriture invalide en ligne 32, pas ce buffer.

---

## 5. Corrections d'explications IA erronées (obligatoire pour l'exercice)

### 🔴 Erreur IA #1 — confusion « still reachable » et fuite

**Explication IA typique (incorrecte) :**
> « Valgrind détecte une fuite mémoire dans `crash_example` : 1 024 octets ne sont jamais libérés (`still reachable: 1,024 bytes`), en plus du crash. »

**Pourquoi c'est faux :** *"still reachable"* est **explicitement une catégorie distincte** d'une fuite dans la terminologie Valgrind — elle signifie qu'un pointeur valide existait toujours vers ce bloc à la sortie du programme (ici, interrompue par le signal). Une vraie fuite nécessite qu'**aucune** chaîne de pointeurs ne mène plus au bloc (comme le cas *definitely lost* de la section 2). De plus, le compteur `ERROR SUMMARY: 1 errors` confirme qu'un seul problème a été détecté par Valgrind — l'écriture invalide en L32 — et non deux comme le suggérerait l'explication naïve. Le bloc *still reachable* n'est même pas listé comme erreur ici (Valgrind ne l'affiche qu'en information, sauf avec des options supplémentaires comme `--errors-for-leak-kinds=all`).

### 🔴 Erreur IA #2 — confondre use-after-free et fuite mémoire

**Explication IA typique (incorrecte) :**
> « `aliasing_example` a une fuite mémoire causée par le `free(a)` suivi d'accès via `b`. »

**Pourquoi c'est faux :** le `HEAP SUMMARY` du programme dit explicitement `"All heap blocks were freed -- no leaks are possible"`. Il n'y a **aucune** fuite ici : le bloc a bien été libéré (une seule fois, correctement). Le problème n'est pas qu'on a *oublié* de libérer quelque chose, c'est qu'on a **continué à utiliser** un pointeur (`b`) après la libération de l'objet qu'il référence — ce qui est un **use-after-free**, une catégorie d'erreur orthogonale à la fuite. Confondre les deux mène à un mauvais diagnostic : corriger une "fuite" supposée pousserait à *ajouter* un `free()`, alors que le vrai correctif ici est d'éliminer les accès à `b` après la ligne 38 (par exemple en mettant `b = NULL;` juste après `free(a);`, pour transformer un bug silencieux en crash immédiat et détectable au premier accès).

---

## 6. Synthèse des catégories Valgrind rencontrées

| Terme précis | Programme concerné | Signification exacte |
|---|---|---|
| `Invalid read/write` sur bloc *free'd* | `aliasing_example` | Use-after-free : accès à un objet dont la durée de vie est terminée, mais l'adresse reste dans les anciennes bornes de l'allocation |
| `Invalid write` sur adresse `0x0` | `crash_example` | Déréférencement de pointeur NULL : accès à une adresse qui n'a jamais appartenu à un objet |
| `definitely lost` | `heap_example` | Fuite au sens strict : plus aucun pointeur vivant ne référence le bloc à la sortie |
| `still reachable` | `crash_example` | Bloc non libéré mais toujours référencé par un pointeur valide au moment de l'arrêt — n'est pas comptée comme une erreur par défaut |
| `0 errors` / heap trivial | `stack_example` | Absence de mauvais usage du tas suivi par Memcheck — ne valide pas la pile |


### My personal Corrections

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
