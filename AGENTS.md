# Agent instructions

Before doing any work on the Devilution C23/UI migration in this repository, read `docs/audit/LIBGUMBALL_WORKFLOW.md` completely and follow it as the canonical operating procedure.

Critical continuity rules:

- Work only in `Haikuno/libgumball` and `Haikuno/devilution-revamp`. Never commit to libGimbal or any other repository.
- Reuse the single draft validation PR #7. Do not create another validation PR and do not merge PR #7 without explicit user approval.
- Do not repeatedly rediscover GitHub tooling. If run/job IDs or the needed GitHub action are already known, invoke that action directly. Known workflow job IDs should go straight to job-log fetching rather than another CLI/tool-discovery loop.
- Every concrete task has a hard 15-minute limit. Also stop early after 3 consecutive blocked/failed attempts on the same obstacle.
- On either limit, perform the emergency exit defined in `docs/audit/LIBGUMBALL_WORKFLOW.md`: stop, preserve useful work/evidence, make a safe checkpoint commit if appropriate, mark incomplete work clearly, record exact remaining state and next action, and report the interruption to the user.
- Never make an unsafe or misleading checkpoint merely to satisfy the emergency-exit rule.
- Preserve and resume from verified checkpoints instead of restarting investigation from scratch after interruption.
- Do not suppress allocator warnings, weaken libGimbal's allocation tracker, or modify libGimbal to hide a libGumball lifecycle problem.
- Do not start the Devilution UI port until the libGumball framework foundation is solid.

The full workflow file contains the authoritative details for CI, validation, dependency policy, rendering parity, allocator investigation, commits, and handoff discipline.
