# frame-hook-example

A minimal example of the **frame hook API**, a proposal for CPython
([`PyUnstable_AddFrameHook`](https://github.com/guilhermeleobas/cpython)).

## What is a frame hook?

A frame hook is a C callback that CPython invokes every time it is about to
execute a Python frame (i.e. every function call). The hook receives the
`_PyInterpreterFrame` as argument and returns a `PyCodeObject`, which can be either
the original one unchanged, or a brand-new one to execute instead.

It is an alternative to the eval frame API
(`_PyInterpreterState_SetEvalFrameFunc`) with one key difference: CPython
remains responsible for all frame management. The hook does not evaluate the
frame itself — it only decides which code object to run. Creating the new
frame, copying arguments, and executing the bytecode are all handled by
CPython as usual.

Multiple hooks can be registered at once; CPython runs them in order, feeding
the output of one as the input to the next.

## This example

`hook.c` registers a hook that watches for a function named `add` and swaps
its code object with the one from `sub` (subtraction) on the fly. From
Python's perspective, `add` is called normally — the swap is invisible.

`demo.py` wraps the hook in a `contextlib.contextmanager` so it is active
only inside a `with` block:

```
=== no hook ===
add(10, 3) = 13

=== hook installed (add swapped to sub) ===
add(10, 3) = 7

=== hook removed ===
add(10, 3) = 13
```

## Build & run

Requires a [CPython build](https://github.com/python/cpython/compare/3.14...guilhermeleobas:cpython:guilhermeleobas/hook) with the frame hook code.

```bash
make run
```
