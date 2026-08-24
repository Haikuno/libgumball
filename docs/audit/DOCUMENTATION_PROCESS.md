# Documentation process

## Goal

A fresh agent should be able to resume safely from a minimal prompt such as:

`work on Haikuno/libgumball, branch <name>`

The repository—not chat history—must carry the continuation state.

## Required document model

### `AGENTS.md`
Permanent agent rules and mandatory reading order only.

Do not put changing project status or investigation history here.

### `CURRENT_STATE.md`
**Mandatory branch-local handoff.** This is the first dynamic document an agent reads.

It must stay short and answer:

- What branch/phase is this?
- Is the current gate GO or NO-GO when relevant?
- What is already complete and should not be reopened casually?
- What blockers remain?
- What is validated versus only source-reviewed?
- What exact action should the next agent perform?

Update it whenever phase, blockers, validation state, or next action change. Do not add chronological history.

### `docs/audit/LIBGUMBALL_WORKFLOW.md`
Stable operating procedure: development loop, validation order, CI discipline, quality rules, anti-stall behavior, and handoff requirements.

Avoid project-specific historical debugging details.

### `docs/audit/DOCUMENTATION_PROCESS.md`
Rules for maintaining the documentation system itself. Agents only need to read this when editing documentation/process.

### `docs/audit/archive/`
Historical evidence only: resolved investigations, old checkpoints, rejected approaches worth preserving, and retired task lists.

Archives are **not** mandatory reading. Active docs should link to a specific archive only when that history is needed for a current blocker.

## No duplicate status/onboarding files

There should be one active continuation source: root `CURRENT_STATE.md`.

Do not create new `CHECKPOINT.md`, `PROGRESS.md`, `STATUS.md`, `QUICKSTART.md`, or similarly overlapping active files for each agent/session. `AGENTS.md` already defines onboarding; `CURRENT_STATE.md` defines current work.

If temporary notes are needed during work, fold the durable result into `CURRENT_STATE.md` or an archive before handoff and remove the temporary file.

## Cleanup rules

At every meaningful milestone or phase change:

1. update `CURRENT_STATE.md`;
2. remove obsolete TODOs and completed blockers from active docs;
3. remove references to deleted/staging/task branches;
4. remove old SHAs described as current;
5. archive useful resolved reasoning instead of keeping it active;
6. delete active documents that no longer have a unique role;
7. verify the reading path still leads directly to the next action.

Never delete meaningful reasoning until its durable value is either preserved in an archive or no longer needed because source/tests now embody the contract.

## Efficiency rules

- Prefer decisions, contracts, blockers, and recovery information over narration.
- Do not document every tool call, failed attempt, or commit when Git history already provides it.
- Do not make every agent reread historical investigations.
- Avoid duplicating the same rule across multiple files; keep the authoritative version in the document whose role owns it.
- Keep `CURRENT_STATE.md` small enough to scan in about a minute.

## Branch hygiene and docs

When a short-lived branch is integrated:

1. verify its meaningful commits are preserved;
2. delete the branch;
3. remove its name from active documentation;
4. update `CURRENT_STATE.md` if integration changes the next action.

## Handoff acceptance test

Before ending a meaningful session, mentally start a new chat with no context and only the branch name.

If that agent could not determine the correct next action after reading `AGENTS.md` and `CURRENT_STATE.md`, the handoff is incomplete.
