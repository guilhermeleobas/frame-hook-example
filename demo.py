import sys
from contextlib import contextmanager


def sub(a, b):
    return a - b


@contextmanager
def swapped_add():
    """Context manager that replaces `add` with `sub` for the duration of the block."""
    def hook(frame):
        if frame.f_code.co_name == "add":
            return sub.__code__
        return frame.f_code

    sys.add_frame_hook(hook)
    try:
        yield
    finally:
        sys.remove_frame_hook(hook)


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
