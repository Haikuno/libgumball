# libGumball workflow

Canonical operating procedure for libGumball foundation and Devilution migration work.

## Resume protocol

For every session:

1. confirm the user-requested/current branch and HEAD;
2. read root `CURRENT_STATE.md`;
3. follow the exact next action recorded there;
4. inspect deeper history only when the current task or evidence requires it.

Do not reconstruct state from prior chats when the branch already contains a handoff.

## Branch policy

- Work on the branch the user requested.
- Do not create parallel integration branches.
- Do not create task branches merely for convenience; use them only for genuinely isolated/parallel work.
- After integration, verify meaningful work is preserved, delete the task/staging branch, and remove stale branch references from active docs.

## Development loop

1. Inspect the relevant current source and reproduce the issue when execution is available.
2. Identify an understood cause before editing.
3. Make the smallest **complete** fix; do not trade correctness for small diffs.
4. Run the narrowest useful executable check first.
5. Expand validation only after the narrow check passes.
6. Commit one coherent reason at a time.
7. Adversarially review the batch for ownership, ordering, failure cleanup, backend leakage, API clarity, and unnecessary state.
8. Update `CURRENT_STATE.md` before handoff if continuation changed.

If execution is unavailable, static/source work may continue only where it is genuinely useful. Do not invent speculative fixes merely because validation cannot run.

## Validation order

When validating a foundation batch, prefer staged proof rather than one giant first run:

1. SDL3 configure/build and focused affected tests.
2. raylib configure/build and focused affected tests.
3. full registered CTest trees.
4. sanitizers for lifecycle/memory work (SDL3 ASan/UBSan in the current foundation flow).
5. backend restart/rendering parity regressions affected by the batch.

Rendering parity is exact; do not add tolerances to manufacture a pass.

## CI policy

Development is local-first. Hosted CI is checkpoint validation, not the debugging loop.

- One intentional run per meaningful checkpoint is normally enough.
- Do not repeatedly poll Actions or repeatedly fetch raw logs.
- Inspect status once; if a failure needs detail, use the narrowest available job/step/artifact evidence.
- Never create duplicate push/PR/manual/sentinel validation for the same checkpoint.

## Test and allocator rules

- Tests must clean up owned state on failure whenever practical so one failure does not contaminate later diagnostics.
- Any failure or diagnostic proven to be owned by the unmodified pinned libGimbal dependency is non-blocking for libGumball.
- Do not fork, patch, or work around libGimbal bugs in this workstream; assume dependency-owned bugs will be corrected in libGimbal.
- LibGumball-owned sanitizer failures, failing tests, and invalid-pointer diagnostics remain errors.
- Informational remaining-allocation counters are not automatically test failures.
- Do not prewarm types/registries, switch contexts, reorder initialization, or weaken tracking merely to change allocator output.
- A LeakSanitizer suppression is acceptable for a source-proven libGimbal-owned issue when needed to keep libGumball validation usable. Do not use dependency suppressions to hide libGumball-owned leaks.

## Architecture and quality

- libGumball remains generic; Devilution-specific concepts belong in Devilution.
- SDL3 and raylib are first-class backends; shared abstractions must remain backend-neutral.
- Ownership, lifetime, input, resource, and lifecycle contracts should be understandable from APIs/source, not initialization folklore.
- A passing build does not by itself prove architectural quality.
- Fix root causes; remove workaround machinery rather than normalizing it.

## Anti-stall rule

Each concrete step has a hard limit of 15 minutes or 3 consecutive blocked/failed attempts on the same obstacle, whichever comes first.

At the limit:

1. stop that path;
2. preserve useful work/evidence;
3. commit only if the partial state is safe and understandable;
4. update `CURRENT_STATE.md` with the blocker and best next action when continuation changed;
5. do not repeat the same failed tool/search/build loop.

## Documentation and handoff

Active docs contain current rules/state only. Historical investigations belong under `docs/audit/archive/`.

Before ending a meaningful session, remove stale references discovered during the work and ensure `CURRENT_STATE.md` answers: **what should the next agent do now?**

See `docs/audit/DOCUMENTATION_PROCESS.md` for maintenance rules.
