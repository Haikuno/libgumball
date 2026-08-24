# Current state

Branch: `devilution/full-game-ui`
Phase: foundation complete / Devilution integration ready
Gate: **GO**

PR #8, `Finalize libGumball framework foundation`, is merged. Foundation architecture, ownership, input/navigation, backend abstraction, resource management, hierarchy/layout, core types, and regression coverage are accepted unless new evidence exposes a regression.

## Validation

The pre-rewrite source checkpoint `9f548d37480f2d4f1ce4e26eacfdb523692ffa55` passed GitHub Actions run `32673141815` with SDL3/raylib full tests, C23 overload compilation, ASan/UBSan, backend restart tests, and exact rendering parity.

Final manual SDL3 regression validation is complete and passing:

- `GumballTests` passes when launched directly from `build/tests` and from an unrelated working directory;
- text remains consistently sharp while scrolling and during width/height focus animations;
- width/height animation continuously reflows affected Container layout;
- exposed outer and inner Container backgrounds scroll their respective Containers;
- hovering a child Button scrolls its nearest Container without changing mouse focus semantics.

Pinned libGimbal: `4be883629d5d2b24f9b4fd790da142e2dd7ec964`.

Confirmed bugs owned by the unmodified pinned libGimbal dependency remain non-blocking for libGumball and must not be patched or worked around here. This includes the raw GblObject property-varargs type-safety issue where an integer argument supplied for a float property can compile while violating the variadic type contract. LibGumball-owned failures remain blocking.

## Next action

Continue libGumball/Devilution integration work from `devilution/full-game-ui`. Do not reopen the completed foundation work without a concrete regression or new requirement.
