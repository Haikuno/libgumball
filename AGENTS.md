# Agent instructions

These instructions are designed so a fresh session can resume from a prompt as small as: `work on Haikuno/libgumball, branch <name>`.

## First action — always

1. Confirm the requested/current branch and HEAD.
2. Read root `CURRENT_STATE.md` **before** broad code/history investigation.
3. Read `docs/audit/LIBGUMBALL_WORKFLOW.md`.
4. Read `docs/audit/DOCUMENTATION_PROCESS.md` only when changing documentation or process.

`CURRENT_STATE.md` is the branch-local handoff. It must state the current phase, blockers, validation state, and exact next action. Do not reconstruct project state from old chats when the repository already records it.

Do not begin by reading archives or replaying old investigations unless `CURRENT_STATE.md` or new evidence requires it.

## Source-of-truth rules

- Work on the branch the user requested. Do not silently switch to another branch.
- The requested branch plus its `CURRENT_STATE.md` are the operational source of truth.
- Do not create parallel integration branches. Use short-lived task branches only when isolation is genuinely useful, then integrate and delete them promptly.
- Never lose meaningful work when cleaning branches or documentation; verify it is preserved first.

## Work rules

- Work only in approved repositories. Never modify libGimbal to hide libGumball problems.
- Any failure or diagnostic proven to be owned by the unmodified pinned libGimbal dependency is non-blocking for libGumball. Do not fork, patch, or work around libGimbal here; assume dependency-owned bugs will be fixed in libGimbal. LibGumball-owned sanitizer, test, and invalid-pointer failures remain blocking.
- Development is local-first. Hosted CI is checkpoint validation, not the normal debugging loop.
- Fix root causes, not symptoms. Do not add hacks, hidden production test hooks, diagnostic suppression, or initialization/order tricks.
- Keep changes focused and independently understandable.
- Follow the 15-minute / 3-attempt anti-stall rule in `LIBGUMBALL_WORKFLOW.md`.
- Do not begin the Devilution UI migration until the current foundation status explicitly permits it.

## Authorship and simplicity

`master` is the style authority. New code should look like Haikuno wrote it and polished it heavily.

- Prefer the smallest clear implementation that preserves the required architecture and behavior.
- Add guards, bookkeeping, abstractions, and tests only for plausible API use, a demonstrated regression, or a material correctness/lifetime requirement.
- Do not harden against contrived internal callback sequences merely because they are technically constructible.
- Do not create generic helper layers until real duplication makes the result simpler.
- Tests should protect public behavior and meaningful regressions, not implementation trivia.
- Keep comments and Doxygen short and purposeful. Do not narrate obvious control flow or internal reasoning.
- Preserve useful error handling, logging, ownership protection, backend parity, and diagnostics; simplicity is not an excuse to weaken real correctness.
- When existing integration code is more elaborate than necessary, simplify it without losing useful functionality.

## Quality standard

A passing build is necessary but not sufficient. Review architecture, API clarity, ownership/lifetime, error handling, backend separation, portability, performance design, test quality, maintainability, and documentation accuracy.

A known material defect means the affected quality area is not finished. Prefer removing workaround machinery over normalizing it.

## Handoff and documentation — mandatory

Before ending a meaningful work session:

- update `CURRENT_STATE.md` if the phase, blocker, validation state, or next action changed;
- remove stale active-document references discovered during the work;
- move useful history out of active docs into `docs/audit/archive/`;
- ensure the next agent can continue without chat history or a large prompt.

Never let active documentation become a chronological diary. Preserve decisions and recovery information, not every intermediate attempt.
