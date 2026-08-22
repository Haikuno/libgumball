# libGumball Migration Workflow

Canonical operating procedure for the Devilution C23/UI migration work on libGumball.

## Repository and branch safety

- Work only in `Haikuno/libgumball` and `Haikuno/devilution-revamp`.
- Never commit to libGimbal or any other repository.
- libGumball migration work uses `devilution/full-game-ui` unless the user explicitly changes it.
- Devilution migration notes use `migration/c23-libgimbal`.
- Never merge a pull request unless the user explicitly asks for it.
- Do not begin the Devilution UI port until the libGumball framework foundation is solid.

## Development loop

Development is local-first.

1. Inspect the current branch and relevant repository evidence.
2. Reproduce locally when an executable environment is available.
3. Make one focused change for one understood cause.
4. Run the narrowest relevant local test first, then broader configure/build/test/sanitizer/parity validation as appropriate.
5. Commit small, independently understandable changes.
6. Use hosted CI as checkpoint validation, not as the normal debugging loop.
7. Record meaningful validated framework changes in the Devilution-side audit history.

If local execution is unavailable, continue from repository/static evidence. Do not automatically substitute hosted CI for local iteration.

## Hosted CI

The reusable CI workflow covers:

- SDL3 build + tests
- raylib build + tests
- SDL3 ASan + UBSan
- exact SDL3/raylib pixel parity

Independent backend jobs run in parallel.

### Trigger policy

- Ordinary `devilution/full-game-ui` source commits do **not** automatically run hosted CI.
- Changes under `.github/workflows/**` invoke the migration validation wrapper because the workflow itself is being changed.
- `workflow_dispatch` is the normal explicit manual validation path for clients that can dispatch workflows directly.
- ChatGPT's connected GitHub integration cannot currently dispatch a workflow directly. For that client only, updating `.github/ci-trigger` is the explicit manual sentinel for one validation run. Treat it exactly like pressing a manual-run button; do not update it during ordinary source iteration.
- `master` receives post-merge push CI.
- Do not create duplicate PR, push, sentinel, or manual runs for the same checkpoint.
- Cancel superseded runs when a newer checkpoint makes an older run irrelevant.

A meaningful completed batch may receive one hosted validation run even when the user did not separately request CI.

### Hosted diagnostics

Do not poll Actions or repeatedly retrieve logs.

1. Read the connector-visible commit status once.
2. If necessary, inspect job/step conclusions to identify the failing phase.
3. Raw job logs get at most one retrieval attempt per failing job.
4. If raw logs are unavailable or awkward, immediately use the focused failure artifact produced by that job.
5. Once usable evidence exists, stop querying CI and work from that evidence.

The migration wrapper publishes a `libgumball/ci` commit status containing the exact run URL so connected clients do not need to discover runs indirectly.

## Test-framework policy

libGumball uses libGimbal's `GblTestScenario` / `GblTestSuite` framework according to libGimbal's own normal usage pattern.

- The test runner should remain simple: backend setup, scenario creation, suite registration, scenario execution, backend teardown.
- Test suites own the runtime objects they need through normal suite/case fixtures.
- Do not prewarm types, classes, signals, pools, roots, widgets, or other dependency internals merely to influence allocation-tracker output.
- Do not pin classes or transfer global storage between allocation contexts from the test runner unless libGimbal's documented public contract explicitly requires it.
- Do not couple unrelated test executables solely to compensate for a CI build command; build the registered test tree instead.
- A failing test must still clean up resources it acquired whenever practical so one failure does not contaminate later diagnostics.

### Allocation tracker semantics

Treat allocator diagnostics according to what libGimbal actually guarantees.

- `[Allocation Tracker] Attempt to free unknown pointer` is an error that requires investigation.
- `[Allocation Tracker] Attempt to realloc unknown pointer` is an error that requires investigation.
- Similar invalid-pointer diagnostics are errors.
- `Remaining Allocs` / `Remaining Bytes` are diagnostic counters printed by `GblTestScenario`; libGimbal does not define a nonzero value by itself as scenario failure. Do not invent a CI failure rule around those totals.
- For suspected leaks, prove ownership and lifetime, then use normal object assertions plus ASan/LeakSanitizer. Do not manipulate dependency initialization order simply to force tracker totals to zero.

When a dependency intentionally or defectively retains process-global memory outside libGumball ownership, a LeakSanitizer suppression is allowed only when all of the following are true:

- the allocation site is source-proven;
- the allocation belongs to unmodified libGimbal rather than libGumball runtime state;
- the suppression is symbol-specific and documented;
- allocator unknown-pointer diagnostics remain unsuppressed.

Never modify libGimbal to hide a libGumball ownership problem.

## Local validation

When execution is available, validate both backends affected by a change.

- SDL3 is the default and must remain first-class.
- raylib must remain equally supported.
- Run the sanitizer configuration for lifecycle/memory work when the toolchain supports it.
- Backend-specific interactive `dev.c` programs are development tools, not automated CTest cases.
- Backend problems belong at the backend abstraction boundary; do not hide them in application or test code.

## Rendering parity

Backend parity is exact: zero differing pixels.

- Do not introduce arbitrary image tolerances to obtain a pass.
- Keep explicit PASS/FAIL output.
- Use meaningful artifact names such as `backend-parity-sdl3.png`, `backend-parity-raylib.png`, and `backend-parity-diff.png`.
- Expand the parity scene progressively across clipping, alpha, textures, fonts, borders, rounded geometry, and other shared rendering behavior.

## Dependencies

- Pin vcpkg/toolchain revisions instead of following floating upstream state.
- Treat dependency upgrades as explicit changes.
- Cache dependency builds/downloads where practical.
- Install only prerequisites justified by the actual dependency graph or build evidence.
- Do not remove an apparently indirect prerequisite without checking what the selected port actually invokes.

## Anti-stall rule

Every concrete step has a hard limit of 15 minutes or three consecutive blocked/failed attempts on the same obstacle, whichever comes first.

A concrete step is one bounded unit such as diagnosing one failure, implementing one fix, validating one change, or proving one ownership path. It is not the whole user request.

When the limit is reached:

1. Stop that investigation path immediately.
2. Preserve all useful evidence and completed work.
3. Commit only if the partial state is safe and understandable; never fabricate a clean checkpoint.
4. Record the exact current SHA, verified findings, unfinished work, blocker, and best next action.
5. Tell the user what stopped and what was preserved.
6. Resume later from the verified checkpoint rather than rediscovering the same state.

Never remain in a repeated tool, search, reasoning, build, or CI loop.

## Commit discipline

- Prefer one reason per commit.
- Each commit should compile/test independently when practical.
- Do not mix unrelated CI, backend, allocator, and framework-feature changes.
- Comments should explain non-obvious contracts, not narrate implementation history.
- Prefer deleting workaround machinery over documenting it as permanent architecture.

## Migration architecture

- libGumball is a generic UI framework; Devilution-specific concepts remain in Devilution.
- Backend abstraction should be genuinely backend-neutral. If an abstraction becomes backend-shaped, redesign it instead of adding special cases.
- Ownership, lifetime, input, resource, and lifecycle contracts should be understandable from public APIs and local implementation—not from initialization folklore.

## Handoff and audit history

`Haikuno/devilution-revamp` `docs/audit/LIBGUMBALL_CHANGES.md` is the canonical migration ledger.

- Record meaningful, validated framework changes with their exact libGumball commit SHA/range.
- Do not label speculative or statically reviewed changes as runtime-validated.
- Preserve enough detail that a later session can resume without repeating root-cause investigation.
- If the connected GitHub interface cannot safely update the large canonical ledger without replacing truncated content, preserve the checkpoint in an adjacent audit file rather than risking ledger corruption; merge it into the canonical ledger later through a safe editing path.
