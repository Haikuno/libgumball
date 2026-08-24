# Current state

PR source: `integration/foundation-final`
Continuation branch after merge: `devilution/full-game-ui`
Phase: foundation final / regression fix
Promotion: **pending manual SDL3 regression validation**

## Reviewable history

The foundation branch is organized into a small subsystem-oriented history for manual review. Review the original foundation source commits in order:

1. input and navigation
2. resource manager ownership
3. widget hierarchy and elements
4. backend abstraction and rendering
5. core value types
6. foundation test-suite registration
7. container hover scrolling regression fix

The housekeeping commit contains only build/test wiring, CI, documentation, and fixtures.

Post-review manual SDL3 testing added two focused fixes:

- SDL3 text draw positions are snapped to the pixel grid before `TTF_DrawRendererText`, eliminating the severe glyph-quality changes caused by fractional animated/scrolled positions. Manual testing confirmed the pixel snap fixes the reported text rendering issue.
- Widget geometry property changes now refresh layout when `x`, `y`, `w`, `h`, relative positioning, border width, or border radius changes. Animated geometry therefore reflows parent/self Container layout on each applied value instead of waiting for an unrelated scroll/layout update. `Widget::animateLayout` covers child-size reflow and top-level Container self-reflow.

The pre-rewrite source checkpoint `9f548d37480f2d4f1ce4e26eacfdb523692ffa55` passed GitHub Actions run `32673141815` with SDL3/raylib full tests, C23 overload compilation, ASan/UBSan, backend restart tests, and exact rendering parity.

Changes after that checkpoint, including hover scrolling, text pixel snapping, and animated-geometry relayout, have not received a new executable checkpoint on this branch. The branch intentionally does not auto-run hosted CI for source-only pushes, its CI wrapper only triggers on `devilution/full-game-ui`, and no local executable checkout is available in the connected environment.

Pinned libGimbal: `4be883629d5d2b24f9b4fd790da142e2dd7ec964`.

Confirmed bugs owned by the unmodified pinned libGimbal dependency are non-blocking for libGumball and must not be patched or worked around here. LibGumball-owned failures remain blocking.

## Next action

Manually verify the current SDL3 nested-Container dev example:

- text remains consistently sharp while scrolling and while focus animations change Button width/height;
- width/height animation continuously reflows affected Container layout rather than updating only after scrolling;
- an exposed `pOuter` background scrolls `pOuter`;
- an exposed inner-Container background scrolls that inner Container;
- hovering a child Button continues scrolling its nearest Container without changing mouse focus semantics.

If those regressions pass, complete manual review and merge PR #8 into `devilution/full-game-ui` when satisfied.

After merge, continue libGumball/Devilution integration work from `devilution/full-game-ui`. Delete `integration/foundation-final` after verifying the merge preserves the rewritten commits.
