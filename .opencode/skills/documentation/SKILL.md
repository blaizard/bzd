---
name: documentation
description: Guidelines for adding/updating in-code documentation. Prioritizes self-explanatory code, refactoring complex logic, and minimal single-line comments.
compatibility: opencode
---

# Core Rules

1. **Refactor over documenting:** If code needs comments to explain _what_ it does, it is too complex. Rename variables or extract functions instead.
2. **Document "Why", never "What":** Never rephrase code logic. Only document non-obvious context, business rules, or workarounds.
3. **Intent over implementation:** Describe the goal, not the methods or classes, those rot on rename.
4. **Single lines only:** Prefer one-line comments over blocks or verbose docstrings. Even when combining fields (e.g. Doxygen brief + detailed), keep the total to one line.
5. **Document public API:** Give every public function or API a clear, concise description of its purpose.
6. **Keep comments honest:** After editing code, verify existing comments still hold true. Fix stale or grammatically off ones, and delete any that no longer make sense.
