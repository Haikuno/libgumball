# Agent instructions

Before doing any work on the Devilution C23/UI migration in this repository, read `docs/audit/LIBGUMBALL_WORKFLOW.md` completely and follow it as the canonical operating procedure.

Critical continuity rules:

- Work only in `Haikuno/libgumball` and `Haikuno/devilution-revamp`. Never commit to libGimbal or any other repository.
- Normal development is local-first. Do not use hosted GitHub Actions merely because a commit was pushed or a PR exists.
- During normal iteration, prefer local configure/build/test/sanitizer/parity validation whenever an executable environment is available. Hosted CI is not the normal development loop.
- When wrapping up a meaningful batch or checkpoint, one hosted CI validation run is allowed and recommended when the workflow supports it, even if the task was not specifically about CI.
- On clients that can dispatch workflows directly, use `workflow_dispatch`. The connected GitHub integration used by ChatGPT cannot currently dispatch it directly, so `.github/ci-trigger` is the explicit connector-friendly manual sentinel: updating that file intentionally starts one migration-branch validation run. It is not an ordinary source-change trigger.
- Never use GitHub Actions as an automatic fallback simply because local execution is unavailable during normal iteration. Continue from repository evidence, then use the single wrap-up CI run when appropriate.
- Feature-branch source commits should not automatically consume hosted CI. On `devilution/full-game-ui`, only workflow-file changes or an intentional `.github/ci-trigger` sentinel update invoke the migration validation wrapper; `master` receives post-merge push CI.
- Never create duplicate feature-branch push, PR, sentinel, or manual CI runs for the same commit/checkpoint.
- When hosted validation is intentionally used, inspect the run/status once. Raw GitHub Actions job logs get at most one retrieval attempt per failing job. If unusable, immediately switch to job/step metadata and focused failure artifacts; never retry or loop on raw-log retrieval.
- Do not repeatedly rediscover GitHub tooling. Use the narrowest direct repository action needed for the current task.
- Every concrete task has a hard 15-minute limit. Also stop early after 3 consecutive blocked/failed attempts on the same obstacle.
- On either limit, perform the emergency exit defined in `docs/audit/LIBGUMBALL_WORKFLOW.md`: stop, preserve useful work/evidence, make a safe checkpoint commit if appropriate, mark incomplete work clearly, record exact remaining state and next action, and report the interruption to the user.
- Never make an unsafe or misleading checkpoint merely to satisfy the emergency-exit rule.
- Preserve and resume from verified checkpoints instead of restarting investigation from scratch after interruption.
- Do not suppress allocator warnings, weaken libGimbal's allocation tracker, or modify libGimbal to hide a libGumball lifecycle problem.
- Do not reinterpret libGimbal's informational allocation counters as test failures. Unknown-pointer allocator diagnostics are errors; process-lifetime dependency allocations must be evaluated separately and, if suppressed from LeakSanitizer, must be source-proven and narrowly symbol-scoped.
- Do not start the Devilution UI port until the libGumball framework foundation is solid.

## Foundation quality bar

Before adding new framework features or starting the Devilution UI port, treat 10/10 as the required baseline for every non-feature-completion quality category: architecture, API design, internal design, correctness, ownership/lifetime safety, test design, backend abstraction, maintainability, consistency, simplicity, performance design, portability, error handling, documentation/contracts, and build/tooling quality.

- Score the repository as if encountering it anonymously. Do not raise a score because the project is early, ambitious, familiar, or already contains substantial invested work.
- A category is 10/10 only when there is no concrete, material improvement presently justified that would make it meaningfully better without an equal or larger tradeoff. Pure taste, speculative generalization, or extra abstraction without demonstrated benefit does not lower the score.
- If a concrete material defect is known, that category is not 10/10. Do not call green CI or passing tests proof of architectural quality by itself.
- Temporary quality regressions are allowed only inside a bounded active migration with an explicit removal/exit condition. The workstream must not end with that temporary state as accepted debt. A TODO or “fix later” note is not an exit condition.
- Prefer deleting workaround machinery over normalizing it. Never manipulate tests, initialization order, dependency internals, or architecture merely to avoid fixing an owned problem.
- Generalize only when an abstraction represents a real concept already present in the design. Do not add hypothetical abstraction layers for imagined future requirements.
- After every meaningful implementation batch, review it adversarially: look for unnecessary state, duplicate paths, hidden ordering requirements, backend leakage, unclear ownership, representable invalid states, workaround logic, avoidable hot-path cost, or tests that require unnatural setup. A material finding means the batch is not finished.
- Preserve libGumball's existing style and philosophy where they remain compatible with the quality bar; redesign rather than preserve a local convention that materially prevents a 10/10 result.

The full workflow file contains the authoritative details for validation, CI triggering, diagnostics, dependency policy, rendering parity, allocator investigation, commits, and handoff discipline.
