# libGumball Migration Workflow

This file is the canonical operating procedure for the Devilution C23/UI migration work on libGumball.

## Repository and branch safety

- Work only in `Haikuno/libgumball` and `Haikuno/devilution-revamp`.
- Never commit to libGimbal or any other repository.
- libGumball migration work uses branch `devilution/full-game-ui` unless explicitly changed by the user.
- Devilution migration notes use `Haikuno/devilution-revamp`, branch `migration/c23-libgimbal`.
- Never merge a pull request unless the user explicitly says to merge it.

## Pull-request policy

- Use at most one draft PR as the hosted-CI validation window for this branch.
- Reuse PR #7 for every subsequent hosted validation run.
- Do not create replacement PRs merely to obtain fresh CI runs.
- Do not close/recreate the validation PR to clean up CI history.
- Never merge it without explicit user approval.

## Anti-stall rules

- Never stall in tool, CI, search, or inspection loops.
- Once a check has produced usable evidence, advance from that evidence instead of repeating the same check.
- Do not repeatedly rediscover tool schemas or re-query branch/PR/workflow state when the result is already known and still applicable.
- When GitHub run or job IDs are already known, use the direct action for those IDs. In particular, fetch workflow job logs directly instead of rediscovering a CLI or workflow route.
- The connected GitHub integration is action/API based. Do not waste time searching for an interactive `gh` CLI path when the required GitHub action is already available.
- Keep an explicit mental/checkpoint state of the latest known branch SHA, active PR, failing job, failing step, and next action.
- If a tool path is unavailable, choose one alternative path once; if that also cannot provide the missing information, state the limitation and continue with the strongest available evidence.
- Prefer forward progress over redundant verification. Re-check only when a commit, CI rerun, branch change, or new external event could have changed the answer.
- Preserve all completed work and investigation findings when interrupted. Resume from the last verified checkpoint rather than restarting discovery.

### Hard task timebox and emergency exit

Every concrete task has a hard maximum of 15 minutes. A concrete task is one bounded unit such as diagnosing one CI failure, implementing one fix, validating one change, or proving one ownership/lifecycle path.

Trigger the emergency exit immediately when either condition is reached:

- 15 minutes have been spent on the current concrete task without completing it; or
- the same obstacle has produced 3 consecutive blocked or failed attempts, even if 15 minutes have not elapsed.

When the emergency exit triggers:

1. Stop investigating that task. Do not enter another discovery/retry loop.
2. Preserve all useful work and evidence already obtained.
3. If the current changes form a safe, understandable checkpoint, commit them to the current allowed repository/branch with an explicitly incomplete/WIP description when appropriate.
4. If the partial state would be dangerous, misleading, uncompilable in a harmful way, or otherwise not safe to commit, do not fabricate a clean checkpoint. Preserve what can be safely preserved and state clearly what remains uncommitted.
5. Record the exact verified state: current SHA, relevant run/job IDs, failing step/error, files changed, hypotheses proved or disproved, unfinished work, and the best next action.
6. Tell the user that the emergency exit triggered, why it triggered, what was safely saved, and what remains unfinished.

The emergency exit is a safety mechanism for continuity. A session must leave behind a recoverable checkpoint rather than dying while repeatedly trying the same operation.

## Validation loop

1. Inspect the current branch state and latest relevant CI evidence before changing code.
2. Reproduce and validate locally whenever practical.
3. Make one focused change for one understood cause.
4. Run the narrowest relevant local validation first.
5. Run broader local configure/build/tests/parity as appropriate.
6. Commit a small, independently understandable change.
7. Push to the same branch and validate through the same draft PR.
8. Read the actual failing hosted job/step before making another CI-driven fix.
9. Record validated changes in the Devilution-side migration changelog.

Hosted CI is confirmation and platform coverage, not the primary debugging loop.

## Local validation

- Use local validation as the fast iteration loop.
- Run configure, build, tests, and backend parity for the backends affected by a change.
- SDL3 is the default and must remain first-class.
- raylib must remain equally clean and supported.
- Do not hide backend-specific problems in tests or application code; fix the backend abstraction/dependency boundary.
- Keep backend-specific interactive `dev.c` executables separate from automated `GumballTests`.

## CI structure

CI should be split into focused jobs:

- SDL3 build + tests
- raylib build + tests
- backend pixel parity, dependent on the backend jobs
- a sanitizer lane where appropriate

Independent backend jobs should run in parallel.

Cancel superseded runs when a newer commit on the same PR makes an older run irrelevant.

## Dependency policy

- Pin vcpkg/toolchain revisions instead of following floating upstream state.
- Treat dependency upgrades as explicit changes.
- Cache vcpkg downloads/binaries/packages where practical so ordinary validation spends time on libGumball rather than rebuilding unchanged dependencies.
- Install only prerequisites justified by the actual dependency graph or build logs.
- Do not remove a prerequisite merely because it appears indirect; verify what the relevant vcpkg port actually invokes.

## Diagnostics and sanitizers

- Do not suppress allocator warnings or weaken libGimbal's allocation tracker.
- Promote known correctness diagnostics to explicit CI failures when useful.
- Prefer focused gates over globally enabling noisy warning policies that mostly expose third-party code.
- Maintain an ASan + UBSan lane for lifecycle/memory correctness where the backend/toolchain supports it cleanly.
- Sanitizers complement the libGimbal allocation tracker; they do not replace it.

## Rendering parity

- Backend parity remains exact: zero differing pixels.
- Do not solve parity failures by introducing arbitrary image tolerances.
- Keep explicit PASS/FAIL output.
- Default artifacts use meaningful names such as:
  - `backend-parity-sdl3.png`
  - `backend-parity-raylib.png`
  - `backend-parity-diff.png`
- Expand parity coverage progressively to include clipping, alpha, textures, fonts, borders, rounded geometry, and other shared rendering behavior.

## Commit discipline

- Prefer small commits with one reason each.
- Each commit should compile/test independently when practical.
- Do not combine unrelated CI, backend, allocator, and feature changes into one commit.
- Do not modify libGimbal to paper over a libGumball ownership/lifecycle bug.
- Comments should be sparse and natural; code and architecture should explain themselves where possible.

## Migration architecture

- libGumball is becoming the full UI framework for Devilution.
- Keep libGumball generic. Devilution-specific concepts remain in Devilution.
- If backend abstraction becomes awkward or backend-specific, redesign it properly rather than adding hacks.
- Do not begin the Devilution UI port until the libGumball framework foundation is solid.

## Handoff and changelog discipline

- `Haikuno/devilution-revamp` `docs/audit/LIBGUMBALL_CHANGES.md` is the migration handoff ledger.
- Record meaningful, validated framework changes there with the corresponding libGumball commit SHA.
- Do not record speculative fixes as validated.
- Keep enough detail that a later session can resume without rediscovering completed work or prior root-cause analysis.

## Current investigation discipline

For allocator/lifetime issues in particular:

- Treat `[Allocation Tracker] Attempt to free unknown pointer` as a real allocator/context mismatch until disproven.
- Trace the exact allocation and free ownership/context before patching.
- Do not suppress the tracker, weaken tests, or paper over the symptom.
- Use the smallest reproducer and first failing test as the primary evidence.
- Once the concrete allocation/free path is established, fix ownership at the correct architectural layer and rerun the existing tests plus sanitizer/hosted CI validation.
