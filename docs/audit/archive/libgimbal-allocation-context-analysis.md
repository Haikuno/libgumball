# libGimbal allocation-context blocker analysis

Status: historical investigation. As of 2026-08-23 these diagnostics are non-blocking by project policy because they are confined to `GblContext`/global-context allocator routing and `GblContext` is planned for deprecation.

Pinned dependency: `gyrovorbis/libgimbal@4be883629d5d2b24f9b4fd790da142e2dd7ec964`.

## Observed diagnostic

Foundation validation previously emitted an allocator diagnostic equivalent to:

`[Allocation Tracker] Attempt to free unknown pointer`

The pattern was backend-independent and occurred around early Root/type/registry activity.

## Source-proven ownership chain

The investigation established the following dependency-level behavior:

1. `GblTestScenario_run()` temporarily installs the test scenario as the global `GblContext` while suites execute.
2. `GblTestScenario` provides allocator tracking through its context.
3. libGimbal process-global registries/containers can be created with a stored `NULL` context before the scenario becomes global.
4. Later operations on those containers enter context-aware allocation code using that stored `NULL` context.
5. An unspecified context is resolved through the current thread/global context; in the relevant implementation `GblThd_context(NULL)` returns `GblContext_global()`.
6. A later hash-set/container growth can therefore allocate replacement storage through the scenario tracker while freeing older storage allocated before that tracker owned the operation.
7. The scenario tracker then observes a free for a pointer whose allocation it never observed, producing the unknown-pointer diagnostic.

This is an allocation-owner/context-boundary mismatch in the dependency, not evidence that libGumball Root-owned runtime objects should change owners.

## Rejected libGumball-side responses

Do not use any of these to make the diagnostic disappear:

- prewarming types, classes, signals, registries, or pools;
- moving backend/test initialization around solely to change allocator ownership timing;
- switching the global context from libGumball tests;
- weakening or suppressing genuine sanitizer or invalid-pointer diagnostics;
- adding a libGumball allocator shim/workaround;
- patching/forking libGimbal for this deprecated-context issue.

Those approaches either hide the ownership boundary or spend framework effort on `GblContext`, which is planned for deprecation.

## Upstream check

Current upstream libGimbal `master` was source-reviewed before considering a dependency bump. The same relevant chain was still present: container/hash-set context can remain unspecified, test scenarios become global context during execution, and unspecified context resolution still flows through the global context.

Therefore a blind update from the pinned revision to current upstream `master` is **not** a source-proven fix. The project instead retains the pinned upstream dependency and treats this `GblContext`-only diagnostic as non-blocking.

## Validation rule

Do not reopen this investigation unless evidence shows a failure outside deprecated `GblContext` allocator routing. Genuine sanitizer failures, failing tests, or actual invalid-pointer diagnostics remain blockers.
