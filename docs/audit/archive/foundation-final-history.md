# Foundation-final history archive

Historical recovery notes for the foundation maturation pass. This file is not mandatory onboarding; current work starts from root `CURRENT_STATE.md`.

## Resolved source issues

### Generic-object hierarchy bridges

Plain `GblObject` nodes are valid transparent hierarchy bridges. Widget state changes apply only to actual Widgets, while reconciliation traverses through generic descendants.

Key source anchor:

- `e640fd2ebd58288d93fd1e1266a424c5039e9d44` — construction-time generic bridge draw reconciliation and focused regression.

Earlier runtime/position hardening on the same branch established the same structural rule for runtime reparenting and relative/absolute position boundaries.

### Container layout failure contract

Container layout was changed from an effectively infallible contract to explicit `GBL_RESULT` propagation. Property/hierarchy/Root/update paths now report reconciliation failure while preserving already-committed source mutation and retry-safe state. Scroll targeting uses cached layout limits instead of invoking fallible recursive layout from input/navigation code.

Key anchors:

- `91dc4411ad0362b9d3c19795b5842cb1c9d5ee87` — coherent Container layout result propagation.
- `84f2d69639cc09005d11cfb0502d7137c93fa5e5` — Root/test compatibility update for the result-bearing contract.

### Manager explicit-eviction reentrancy

Manager cache eviction protects the extracted cache-entry ownership state before releasing callback-capable resources. Nested eviction from ordinary finalization can no longer destroy the outer path ownership information.

Key coverage anchor:

- `d6e585cc98b14f754245ed20cbbcd5f0e6ec423e` — production-path finalizer reentrancy regression using `GblRef_activeCount()` to prove the original path reference is not leaked.

### Failure-path regression hygiene

Later test-only hardening ensured deliberately failing regressions disable injected failure state and release locally owned objects before jump-on-failure assertions can abort a case. This covered Manager, Container failure, hierarchy regressions, and the then-active ObjectViewer experiments and removed reliance on ignored `GUM_unref(NULL)` cleanup errors.

Representative range:

- `57308d09dc0f40a42a3e97aee5e477297cbb95b4` through `ac4d4c16374454cb3356fac3bbf137bfa6ec0101`.

## Retired ObjectViewer direction

A transactional ObjectViewer row-rebuild implementation was developed and tested during foundation maturation:

- `4603249b95d8e89be2d18eab5940a43fdb565554` — staged transactional rebuild and rollback coverage.
- `cab82cfe0d2509793107b7367805ce02a55cb3af` — strengthened partial-commit regression precondition.

Executable validation later exposed a rebuild stack overflow. More importantly, the design duplicated responsibilities that belong in reusable Table/Tree primitives. The transactional row engine and its table-like tests were therefore removed from the active foundation rather than repaired further. ObjectViewer now retains only its object/lifecycle contract until `GUM_Table` and `GUM_Tree` exist.

Do not use the old commits above as a template for new ObjectViewer work unless the architecture decision is explicitly revisited.

## Validation history

A hosted checkpoint sentinel commit `21d75642412d7804b4101eeb7ff857edf47f19f9` targeted source commit `91dc4411ad0362b9d3c19795b5842cb1c9d5ee87`.

The immediate single status inspection showed no published status. Per the anti-polling rule, Actions status/log retrieval was not repeatedly queried.

Everything after that source checkpoint remained source/API reviewed rather than executable-validated during the connector-only work. Later sessions added executable SDL3, raylib, and sanitizer validation; current status belongs in `CURRENT_STATE.md`, not this archive.

## Dependency blocker

The active dependency issue is documented separately in:

`docs/audit/archive/libgimbal-allocation-context-analysis.md`

Do not copy its full reasoning back into active status files.

## Archive rule

Add to this file only when historical context materially helps future debugging. Active continuation state belongs exclusively in root `CURRENT_STATE.md`.
