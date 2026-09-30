---
name: write-tests
description: Used whenever you write, generate, update, refactor, review, or debug tests.
---

# Write Tests

Follow these core practices for all test authoring:

## Rules

- Keep tests simple, readable, and easy to maintain. Avoid complex logic, loops, or conditional branching inside test code so failures are obvious.
- Cover the standard path, edge cases (null/empty/boundaries), and expected errors or exceptions.
- Always test outcomes, output values, and observable side-effects—never test private methods or internal implementation details.
- Name tests by context and expected behavior.
- Mock external dependencies. Ensure tests are deterministic and runnable in any order.
- Do not add comments unless strictly necessary. If a test requires comments to explain what it is doing, it is too complex and should be refactored for clarity instead.
