---
name: documentation
description: Guidelines for adding/updating in-code documentation. Prioritizes self-explanatory code, refactoring complex logic, and minimal single-line comments.
compatibility: opencode
---

# Core Rules

1. **Refactor over documenting:** If code needs comments to explain _what_ it does, it is too complex. Rename variables or extract functions instead.
2. **Document "Why", never "What":** Never rephrase code logic. Only document non-obvious context, business rules, or workarounds.
3. **Single lines only:** Prefer one-line comments over text blocks or verbose docstrings. Keep explanations tight and minimal.
