# libGumball Migration Workflow

This file is the canonical operating procedure for the Devilution C23/UI migration work on libGumball.

## Repository and branch safety

- Work only in `Haikuno/libgumball` and `Haikuno/devilution-revamp`.
- Never commit to libGimbal or any other repository.
- libGumball migration work uses branch `devilution/full-game-ui` unless explicitly changed by the user.
- Devilution migration notes use `Haikuno/devilution-revamp`, branch `migration/c23-libgimbal`.
- Never merge a pull request unless the user explicitly says to merge it.

## Hosted GitHub Actions policy

Normal development is local-first. Hosted GitHub Actions is a checkpoint-validation tool, not the normal iteration loop.

- For ordinary source/framework work, configure, build, test, sanitize, and run parity locally when an executable environment is available. Do not query workflow runs, jobs, logs, artifacts, checks, or check suites merely because a commit was pushed or a PR exists.
- During normal iteration, do not substitute hosted Actions for local debugging simply because local execution is unavailable. Report the execution limitation and continue from repository/static evidence.
- A task explicitly about diagnosing, fixing, or validating CI authorizes hosted GitHub Actions during that task.
- Independently of CI-specific work, when wrapping up a meaningful batch or checkpoint, one hosted CI validation run is allowed and recommended when the workflow supports it. This does not require separate current-turn user opt-in.
- Feature-branch source commits should not automatically consume hosted CI. On `devilution/full-game-ui`, a small wrapper may invoke the reusable CI only when `.github/workflows/**` itself changes, so CI-specific workflow edits receive one validation run without making ordinary source commits hosted-CI events.
- `workflow_dispatch` exists for the intentional wrap-up validation run when needed.
- Push CI in the main reusable workflow is restricted to `master` for post-merge coverage. The migration-branch wrapper is limited to workflow-file changes and must not become a general feature-branch push trigger.
- When hosted CI is intentionally used, one hosted run per commit/change/checkpoint is the target. Do not create duplicate PR/push/manual validation for the same feature-branch commit.
- Reuse an existing suitable PR when a PR is otherwise needed, rather than creating replacement PRs solely to obtain fresh runs, and never merge without explicit approval.

### Hosted diagnostics without stalls

Raw GitHub Actions job-log retrieval is a best-effort convenience, not the primary diagnostic path.

- Inspect the hosted run/status once after intentional validation rather than polling it repeatedly.
- Attempt raw job-log retrieval at most once for a failing job.
- If that attempt is blocked, empty, redirected incorrectly, undecodable, or otherwise unusable, do not retry it.
- Switch immediately to normal run/job step metadata to identify the failing step.
- CI steps should be granular enough that step status identifies the failing phase or dependency.
- When exact output is required, preserve focused failure-only diagnostic artifacts from the workflow and inspect those instead of repeatedly fetching raw job logs.
- Do not suppress or weaken failures merely to make diagnostic artifacts easier to obtain.

The repository may still contain GitHub Actions configuration because CI architecture itself is part of the project. Inspecting or editing those files is ordinary repository work and does not by itself authorize repeated hosted execution outside the rules above.

## Anti-stall rules

- Never stall in tool, CI, search, or inspection loops.
- Once a check has produced usable evidence, advance from that evidence instead of repeating the same check.
- Do not repeatedly rediscover tool schemas or re-query branch/PR/workflow state when the result is already known and still applicable.
- Use the narrowest direct repository operation needed for the task.
- The connected GitHub integration is action/API based. Do not waste time searching for an interactive `gh` CLI path when a direct repository action is already available.
- Keep an explicit checkpoint state of the latest known branch SHA, files changed, evidence obtained, remaining blocker, and next action. Track run/job IDs only when hosted Actions is intentionally in use.
- If a tool path is unavailable, choose one alternative path once; if that also cannot provide the missing information, state the limitation and continue with the strongest available evidence.
- Prefer forward progress over redundant verification. Re-check only when a commit, branch change, or relevant external event could have changed the answer.
- Preserve all completed work and investigation findings when interrupted. Resume from the last verified checkpoint rather than restarting discovery.

### Hard task timebox and emergency exit

Every concrete task has a hard maximum of 15 minutes. A concrete task is one bounded unit such as diagnosing one failure, implementing one fix, validating one change, or proving one ownership/lifecycle path.

Trigger the emergency exit immediately when either condition is reached:

- 15 minutes have been spent on the current concrete task without completing it; or
- the same obstacle has produced 3 consecutive blocked or failed attempts, even if 15 minutes have not elapsed.

When the emergency exit triggers:

1. Stop investigating that task. Do not enter another discovery/retry loop.
2. Preserve all useful work and evidence already obtained.
3. If the current changes form a safe, understandable checkpoint, commit them to the current allowed repository/branch with an explicitly incomplete/WIP description when appropriate.
4. If the partial state would be dangerous, misleading, uncompilable in a harmful way, or otherwise not safe to commit, do not fabricate a clean checkpoint. Preserve what can be safely preserved and state clearly what remains uncommitted.
5. Record the exact verified state: current SHA, files changed, hypotheses proved or disproved, unfinished work, blocker, and best next action. Include run/job details only if hosted Actions was intentionally used.
6. Tell the user that the emergency exit triggered, why it triggered, what was safely saved, and what remains unfinished.

The emergency exit is a safety mechanism for continuity. A session must leave behind a recoverable checkpoint rather than dying while repeatedly trying the same operation.

## Default validation loop

1. Inspect the current branch state and relevant repository evidence before changing code.
2. Reproduce and validate locally whenever a local/executable environment is actually available.
3. Make one focused change for one understood cause.
4. Run the narrowest relevant local validation first when possible.
5. Run broader local configure/build/tests/parity when appropriate and available.
6. Commit a small, independently understandable change directly to the allowed branch.
7. If execution is unavailable during iteration, state exactly what was not run; do not switch automatically to hosted GitHub Actions.
8. When wrapping up a meaningful batch/checkpoint, run one hosted CI validation when practical via the intended manual path. Do not duplicate an already suitable run for that commit/checkpoint.
9. Inspect the hosted result once. On failure, use step metadata and focused artifacts; raw job logs get at most one attempt per failing job.
10. Record meaningful validated changes in the Devilution-side migration changelog when appropriate.

## Local validation

- Local validation is the default development loop.
- Run configure, build, tests, and backend parity for the backends affected by a change.
- Run the sanitizer configuration locally when lifecycle/memory correctness is relevant and the toolchain supports it.
- SDL3 is the default and must remain first-class.
- raylib must remain equally clean and supported.
- Do not hide backend-specific problems in tests or application code; fix the backend abstraction/dependency boundary.
- Keep backend-specific interactive `dev.c` executables separate from automated `GumballTests`.

## CI architecture

The following describes the desired project CI configuration. It does not mean every development commit should invoke hosted CI.

CI should be split into focused jobs:

- SDL3 build + tests
- raylib build + tests
- backend pixel parity, dependent on the backend jobs
- a sanitizer lane where appropriate

Independent backend jobs should run in parallel.

Trigger policy:

- ordinary feature-branch source commits: no automatic hosted CI;
- `devilution/full-game-ui` changes under `.github/workflows/**`: one migration-branch push wrapper run that calls the reusable CI, appropriate for CI-specific workflow work;
- meaningful batch/checkpoint wrap-up: one manual `workflow_dispatch` validation run when practical;
- `master` pushes: one post-merge CI run from the main workflow;
- no automatic PR trigger on the migration branch;
- never run duplicate feature-branch validation paths for the same commit/checkpoint.

Cancel superseded runs when a newer commit makes an older run irrelevant.

## Dependency policy

- Pin vcpkg/toolchain revisions instead of following floating upstream state.
- Treat dependency upgrades as explicit changes.
- Cache vcpkg downloads/binaries/packages where practical so intentional hosted validation spends time on libGumball rather than rebuilding unchanged dependencies.
- Install only prerequisites justified by the actual dependency graph or build evidence.
- Do not remove a prerequisite merely because it appears indirect; verify what the relevant vcpkg port actually invokes.

## Diagnostics and sanitizers

- Do not suppress allocator warnings or weaken libGimbal's allocation tracker.
- Promote known correctness diagnostics to explicit failures when useful.
- Prefer focused gates over globally enabling noisy warning policies that mostly expose third-party code.
- Maintain an ASan + UBSan lane for lifecycle/memory correctness where the backend/toolchain supports it cleanly.
- Sanitizers complement the libGimbal allocation tracker; they do not replace it.
- LeakSanitizer suppressions are allowed only for a source-proven dependency process-lifetime allocation site that is outside libGumball ownership. Keep such suppressions symbol-specific, documented, and separate from the libGimbal allocation tracker; never use them to hide libGumball-owned leaks or allocator-context warnings.

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

- Treat `[Allocation Tracker] Attempt to free unknown pointer` or `[Allocation Tracker] Attempt to realloc unknown pointer` as a real allocator/context mismatch until disproven.
- Trace the exact allocation and free/reallocation ownership/context before patching.
- Do not suppress the tracker, weaken tests, or paper over the symptom.
- Use the smallest reproducer and first failing test as the primary evidence.
- Once the concrete allocation/free path is established, fix ownership at the correct architectural layer and rerun available local tests/sanitizers. Use the single hosted wrap-up validation run when appropriate; do not turn hosted CI into the normal debugging loop.
