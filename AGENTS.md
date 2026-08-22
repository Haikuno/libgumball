# Agent instructions

Before doing any work on the Devilution C23/UI migration in this repository, read `docs/audit/LIBGUMBALL_WORKFLOW.md` completely and follow it as the canonical operating procedure.

Critical continuity rules:

- Work only in `Haikuno/libgumball` and `Haikuno/devilution-revamp`. Never commit to libGimbal or any other repository.
- Normal development is local-first. Do not use hosted GitHub Actions merely because a commit was pushed or a PR exists.
- A task explicitly about diagnosing, fixing, or validating CI authorizes hosted GitHub Actions for that task. Outside CI-specific work, hosted Actions requires explicit current-turn user opt-in.
- Never use GitHub Actions as an automatic fallback when local execution is unavailable. State what could not be executed and continue from repository evidence instead.
- Feature-branch source commits should not automatically consume hosted CI. CI workflow edits may receive one PR-triggered run; explicit hosted validation uses the manual workflow; `master` receives post-merge push CI.
- Never create duplicate feature-branch push and PR CI runs for the same commit.
- Raw GitHub Actions job logs get at most one retrieval attempt per failing job. If unusable, immediately switch to job/step metadata and focused failure artifacts; never retry the raw-log route.
- Do not repeatedly rediscover GitHub tooling. Use the narrowest direct repository action needed for the current task.
- Every concrete task has a hard 15-minute limit. Also stop early after 3 consecutive blocked/failed attempts on the same obstacle.
- On either limit, perform the emergency exit defined in `docs/audit/LIBGUMBALL_WORKFLOW.md`: stop, preserve useful work/evidence, make a safe checkpoint commit if appropriate, mark incomplete work clearly, record exact remaining state and next action, and report the interruption to the user.
- Never make an unsafe or misleading checkpoint merely to satisfy the emergency-exit rule.
- Preserve and resume from verified checkpoints instead of restarting investigation from scratch after interruption.
- Do not suppress allocator warnings, weaken libGimbal's allocation tracker, or modify libGimbal to hide a libGumball lifecycle problem.
- Do not start the Devilution UI port until the libGumball framework foundation is solid.

The full workflow file contains the authoritative details for validation, CI triggering, diagnostics, dependency policy, rendering parity, allocator investigation, commits, and handoff discipline.
