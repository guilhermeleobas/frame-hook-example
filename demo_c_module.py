"""
Frame hook demo using the C extension module.

This demonstrates the C extension API (hook.set_swap, hook.install, hook.remove_all).
"""

from contextlib import contextmanager
import hook


def sub(a, b):
    return a - b


@contextmanager
def swapped_add():
    """Context manager that replaces `add` with `sub` for the duration of the block."""
    hook.set_swap("add", sub.__code__)
    hook.install()
    try:
        yield
    finally:
        hook.remove_all()


def add(a, b):
    return a + b


# ── without hook ──────────────────────────────────────────────────────────────
print("=== no hook ===")
print(f"add(10, 3) = {add(10, 3)}")   # 13

# ── with hook (add secretly becomes sub) ─────────────────────────────────────
print("\n=== hook installed (add swapped to sub) ===")
with swapped_add():
    print(f"add(10, 3) = {add(10, 3)}")   # 7 (10 - 3) !

# ── hook removed ──────────────────────────────────────────────────────────────
print("\n=== hook removed ===")
print(f"add(10, 3) = {add(10, 3)}")   # 13 again
