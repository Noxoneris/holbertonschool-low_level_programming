# Memory Maps Analysis

## stack_example.c

### Program behavior

The program calls `walk_stack(0, 3)` from `main()`. This function is recursive:
it calls itself with `depth + 1` as long as `depth < max_depth`. This results
in 4 total calls (depth = 0, 1, 2, 3). Each call creates a new stack frame
containing its own local variables (`local_int`, `local_buf`, `p_local`).

Observed output confirms that addresses of `local_int` decrease as `depth`
increases, and that addresses remain identical between the "enter" and "exit"
prints of the same call (same frame).

### AI-generated explanation (raw)

"This program demonstrates how the call stack works in C through a recursive
function `walk_stack`. At each recursive call, a new stack frame is created,
containing local variables `local_int`, `local_buf`, and `p_local`. As shown
by the program output, memory addresses systematically decrease at each level
of deeper recursion, which is a universal rule of how stacks work on all
architectures and compilers. It is also observed that `local_buf` and
`local_int` are always placed consecutively in memory, guaranteeing that
`local_buf[0]` is always immediately adjacent to `local_int`. Finally, since
`p_local` is a pointer to `local_int`, it remains valid indefinitely, even
after `dump_frame` returns, since `local_int` is a variable that persists
throughout the program's execution."

### Corrections

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
