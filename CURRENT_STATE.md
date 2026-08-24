# Current state

PR source: `integration/foundation-final`
Continuation branch after merge: `devilution/full-game-ui`
Phase: foundation final / regression fix
Promotion: **pending scroll-regression validation**

## Reviewable history

The foundation branch is organized into a small subsystem-oriented history for manual review. Review the source commits in order:

1. input and navigation
2. resource manager ownership
3. widget hierarchy and elements
4. backend abstraction and rendering
5. core value types
6. foundation test-suite registration
7. container hover scrolling regression fix

The final housekeeping commit contains only build/test wiring, CI, documentation, and fixtures and does not need source review.

The pre-rewrite source checkpoint `9f548d37480f2d4f1ce4e26eacfdb523692ffa55` passed GitHub Actions run `32673141815` with SDL3/raylib full tests, C23 overload compilation, ASan/UBSan, backend restart tests, and exact rendering parity.

After that validation, manual SDL3 use exposed a mouse-wheel regression: scrolling reused the active-only pointer event target, so inactive-by-default `GUM_Container` backgrounds could not be wheel targets. The focused fix separates geometric hover hit-testing from active event hit-testing. Mouse wheel routing now starts from the topmost interactive widget under the pointer and walks to its nearest scrollable Container, while click/focus targeting remains active-only. `Root::pointerTargets` protects the distinction.

The scroll fix is source-reviewed but has not received a new executable checkpoint on this branch. The branch intentionally does not auto-run hosted CI for source-only pushes, and no local executable checkout is available in the connected environment.

Pinned libGimbal: `4be883629d5d2b24f9b4fd790da142e2dd7ec964`.

Confirmed bugs owned by the unmodified pinned libGimbal dependency are non-blocking for libGumball and must not be patched or worked around here. LibGumball-owned failures remain blocking.

## Next action

Manually verify mouse-wheel scrolling with the reported nested-Container SDL3 example: an exposed `pOuter` background must scroll `pOuter`, an exposed inner-Container background must scroll that inner Container, and hovering a child Button must continue scrolling its nearest Container without changing mouse focus semantics.

If that regression test passes, complete manual review and merge PR #8 into `devilution/full-game-ui` when satisfied.

After merge, continue libGumball/Devilution integration work from `devilution/full-game-ui`. Delete `integration/foundation-final` after verifying the merge preserves the rewritten commits.
