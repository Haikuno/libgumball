# Agent instructions

Before doing any work on the Devilution C23/UI migration in this repository, read `docs/audit/LIBGUMBALL_WORKFLOW.md` completely and follow it as the canonical operating procedure.

Critical continuity rules:

- Work only in `Haikuno/libgumball` and `Haikuno/devilution-revamp`. Never commit to libGimbal or any other repository.
- Hosted GitHub Actions is strictly opt-in for agent operations. Do not query workflow runs, jobs, logs, artifacts, checks, check suites, or use a validation PR unless the user explicitly asks for hosted GitHub Actions diagnostics in the current turn.
- The presence of `.github/workflows/*`, a failing check, a PR, a known run/job ID, or wording such as "CI", "fix CI", or "validation" is not authorization to use hosted Actions. CI work normally means inspecting and editing repository files and validating through available local/static means.
- Never use GitHub Actions as an automatic fallback when local execution is unavailable. State what could not be executed and continue from repository evidence instead.
- Do not repeatedly rediscover GitHub tooling. Use the narrowest direct repository action needed for the current task, but never cross the hosted-Actions opt-in boundary above.
- Every concrete task has a hard 15-minute limit. Also stop early after 3 consecutive blocked/failed attempts on the same obstacle.
- On either limit, perform the emergency exit defined in `docs/audit/LIBGUMBALL_WORKFLOW.md`: stop, preserve useful work/evidence, make a safe checkpoint commit if appropriate, mark incomplete work clearly, record exact remaining state and next action, and report the interruption to the user.
- Never make an unsafe or misleading checkpoint merely to satisfy the emergency-exit rule.
- Preserve and resume from verified checkpoints instead of restarting investigation from scratch after interruption.
- Do not suppress allocator warnings, weaken libGimbal's allocation tracker, or modify libGimbal to hide a libGumball lifecycle problem.
- Do not start the Devilution UI port until the libGumball framework foundation is solid.

The full workflow file contains the authoritative details for validation, dependency policy, rendering parity, allocator investigation, commits, and handoff discipline. Its descriptions of project CI architecture do not authorize an agent to access hosted GitHub Actions.
