# crash_report.md — AI Assisted Crash Report

## 0. Méthodologie

Ce rapport est construit **sans exécuter à nouveau le programme** : il s'appuie sur le code source de `crash_example.c` déjà analysé (Task 0), sur la sortie Valgrind réelle fournie par l'utilisateur, et sur le code de sortie du shell (`echo "exit code: $?"` → `139`). Toute affirmation ci-dessous est reliée à une ligne de code précise ou à une preuve d'exécution déjà capturée — aucune n'est déduite en relançant le binaire.

---

## 1. Description du crash

| Élément | Valeur |
|---|---|
| Programme | `crash_example` |
| Signal reçu | `SIGSEGV` (signal 11) |
| Message shell | `Segmentation fault (core dumped)` |
| Code de sortie observé | `139` (= 128 + 11, convention Unix pour "terminé par le signal 11") |
| Ligne fautive | `crash_example.c:32` → `nums[0] = 42;` |
| Type d'opération fautive | **Écriture** (`Invalid write of size 4`, confirmé par Valgrind) |
| Adresse accédée | `0x0` |

---

## 2. Chaîne causale complète (du code source jusqu'au signal)

Le crash n'est **pas** un événement isolé au moment du plantage : c'est l'aboutissement visible d'une décision prise plusieurs lignes plus tôt.

1. **L45** — `int n = 0;` : la variable `n` est initialisée à `0` dans `main`.
2. **L48** — `requesting n=%d` affiche `0`, confirmant que c'est bien ce chemin qui est emprunté.
3. **L30** — `nums = allocate_numbers(n);` appelle la fonction avec `n = 0`.
4. **Dans `allocate_numbers`, L9-10** — la garde `if (n <= 0) return NULL;` est évaluée à **vraie**. La fonction **retourne immédiatement `NULL`**, avant même d'atteindre la ligne `L12` où se trouve le seul `malloc` du programme.
5. **Conséquence directe** : `nums` reçoit la valeur `NULL` (c'est-à-dire l'adresse `0x0`) — **aucune mémoire n'a été allouée**, ni sur le tas ni ailleurs.
6. **L32** — `nums[0] = 42;` est exécutée sans qu'aucune vérification (`if (!nums)`) n'intercepte le cas `NULL`. Cette instruction est équivalente à écrire la valeur `42` à l'adresse mémoire `0x0`.
7. Le CPU tente cette écriture. La MMU (unité de gestion mémoire) constate qu'**aucune page physique n'est mappée** à l'adresse `0x0` dans l'espace d'adressage du processus — cette page est délibérément laissée non mappée par le système d'exploitation, précisément pour détecter ce genre d'erreur.
8. Le noyau intercepte la violation d'accès et envoie le signal `SIGSEGV` au processus.
9. Aucun gestionnaire de signal n'étant installé, le comportement par défaut s'applique : le processus se termine, un core dump est généré, et le shell rapporte `Segmentation fault (core dumped)` avec le code de sortie `139`.

**Ce n'est donc à aucun moment un événement aléatoire ou dépendant de l'environnement** : avec `n = 0` fixé en dur dans le code (L45), ce chemin est emprunté à 100 % des exécutions, sur n'importe quelle machine. C'est un crash **déterministe**, pas un bug intermittent.

---

## 3. Analyse de la cause racine (root cause)

**Cause racine : absence de vérification de la valeur de retour d'une fonction pouvant renvoyer `NULL`.**

Ce n'est *pas* un problème de `malloc` (qui n'est jamais appelé ici), ni un problème de taille de tableau, ni un débordement. C'est un manquement au contrat implicite de `allocate_numbers` : la fonction documente clairement (par son code, L9-10) qu'elle peut renvoyer `NULL`, mais l'appelant (`main`, L30-32) ne teste jamais cette possibilité avant d'utiliser la valeur retournée comme un pointeur valide.

---

## 4. Pourquoi l'accès mémoire est invalide — catégorie précise

**Catégorie d'undefined behavior : déréférencement de pointeur NULL** (*NULL pointer dereference*), à distinguer explicitement d'autres catégories proches :

| Catégorie | Signature caractéristique | S'applique ici ? |
|---|---|---|
| Use-after-free | Accès à une adresse qui **appartenait** à un bloc, "N bytes inside a block of size M, free'd" | ❌ Non — aucun `free` n'a eu lieu avant ce point sur ce pointeur |
| Heap-buffer-overflow | Accès juste **après** (ou avant) un bloc valide, "N bytes after a block of size M, alloc'd" | ❌ Non — il n'y a jamais eu de bloc alloué pour `nums` |
| **NULL pointer dereference** | Adresse `0x0`, décrite par Valgrind comme *"not stack'd, malloc'd or (recently) free'd"* | ✅ **C'est exactement le cas ici** |
| Stack overflow | Épuisement de l'espace de pile par récursion/allocation excessive | ❌ Non — Valgrind mentionne cette hypothèse dans son message générique de bas de rapport (*"if you believe this happened as a result of a stack overflow... unlikely but possible"*), mais c'est un texte **standard** affiché à chaque SIGSEGV, pas une détection réelle. Il n'y a aucune récursion ni gros tableau local dans ce programme — cette piste doit être écartée. |

La preuve technique vient directement de la sortie Valgrind fournie :
```
Invalid write of size 4
   at 0x400128A: main (crash_example.c:32)
 Address 0x0 is not stack'd, malloc'd or (recently) free'd
```
La formulation *"is not stack'd, malloc'd or (recently) free'd"* est la signature Valgrind spécifique d'une adresse qui **n'a jamais été un objet valide d'aucune sorte** — ni variable locale, ni bloc heap, ni bloc libéré récemment. C'est le marqueur distinctif d'un pointeur NULL (ou plus généralement d'un pointeur non initialisé/invalide pointant hors de tout objet), par opposition aux autres catégories du tableau ci-dessus qui, elles, référencent un objet réel mais mal utilisé.

**Mémoire stack ou heap ?** Ni l'une ni l'autre au sens strict : l'adresse `0x0` ne fait partie d'aucun segment mappé du tout (le résumé Valgrind le confirme : `HEAP SUMMARY: total heap usage: 1 allocs, 0 frees` — le seul `malloc` du programme entier, celui de `L12` dans `allocate_numbers`, **n'a jamais été exécuté** puisque la garde L9 l'a court-circuité ; le `1 alloc` compté par Valgrind correspond à un buffer interne `stdio`, sans rapport avec `nums`). L'écriture fautive cible donc une adresse **hors de tout objet mémoire du programme**, stack et heap confondus.

---

## 5. Évaluation critique de propositions générées par IA

En simulant ce qu'un outil IA générerait typiquement pour "causes possibles" et "correctifs possibles" face à ce crash, voici une évaluation critique explicite de chaque piste :

| Proposition IA typique | Verdict | Justification |
|---|---|---|
| « Le crash vient d'un échec de `malloc` (mémoire insuffisante) » | ❌ **Incorrecte** | `malloc` n'est jamais appelé (voir §2, étape 4). Le `NULL` provient d'une garde logique (`n <= 0`), pas d'un échec d'allocation système. Cette proposition est plausible en apparence (un `malloc` qui échoue renvoie aussi `NULL`) mais ne correspond pas au chemin d'exécution réel de ce programme. |
| « C'est un débordement de tampon (`nums[0]` hors bornes) » | ❌ **Incorrecte** | Il n'existe aucun tampon alloué pour `nums` (voir tableau §4) : on ne peut pas "déborder" d'un objet qui n'a jamais existé. La sortie Valgrind confirme l'absence de tout bloc associé à cette adresse. |
| « C'est potentiellement un dépassement de pile (stack overflow) » | ❌ **Spéculative et incorrecte** | Cette hypothèse s'appuie uniquement sur le message générique standard de Valgrind affiché après *tout* SIGSEGV, pas sur une preuve spécifique à ce programme. Aucune récursion ni grosse structure locale n'existe dans `crash_example.c`. |
| « Le pointeur retourné par `allocate_numbers` n'est jamais vérifié avant utilisation » | ✅ **Correcte et vérifiable** | Confirmé directement en lisant L30-32 : aucune instruction `if (!nums)` n'existe entre l'appel et l'utilisation. C'est la seule proposition directement traçable au code source sans hypothèse supplémentaire. |
| « Le programme fonctionnera si on relance l'exécutable » | ❌ **Incorrecte** | Le crash est déterministe (voir §2) : `n = 0` est une constante du code source, pas une entrée variable. Toute ré-exécution produira exactement le même résultat, ce qui est d'ailleurs confirmé par l'utilisateur (capture avec `echo "exit code: $?"` → `139`, cohérent avec l'exécution précédente sous Valgrind). |

**Point méthodologique important :** seule la 4ᵉ proposition ("pointeur jamais vérifié") est vérifiable **par simple lecture du code**, sans avoir besoin de ré-exécuter quoi que ce soit — c'est le critère retenu ici pour distinguer une explication correcte d'une explication spéculative, conformément à l'objectif de l'exercice.

---

## 6. Correctif suggéré (optionnel, clairement identifié comme tel)

**⚠️ SUGGESTION DE CORRECTIF — non demandée comme obligatoire, fournie à titre indicatif :**

```c
nums = allocate_numbers(n);

if (!nums)
{
    printf("  allocation failed or n <= 0, aborting\n");
    return 1;
}

nums[0] = 42;
```

Cet ajout ne "corrige" pas le fait que `n = 0` soit une entrée dégénérée pour ce programme : il transforme un **crash silencieux et brutal** en un **échec contrôlé et explicite**, ce qui est le comportement attendu face à toute fonction dont le contrat autorise un retour `NULL`.

---

## 7. Conclusion

Le segmentation fault de `crash_example` n'est pas "juste un crash" : c'est la conséquence mécanique et prévisible d'un `NULL` produit intentionnellement par une garde de validation (`n <= 0`), jamais vérifié avant d'être déréférencé. La chaîne causale complète (L45 → L9-10 → L30 → L32 → SIGSEGV) est reconstituable entièrement par lecture statique du code, sans avoir besoin de ré-exécuter le programme — ce qui est précisément ce que cet exercice demande de démontrer.
