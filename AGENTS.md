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

The full workflow file contains the authoritative details for validation, CI triggering, diagnostics, dependency policy, rendering parity, allocator investigation, commits, and handoff discipline.
