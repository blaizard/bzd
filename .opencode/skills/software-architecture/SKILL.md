---
name: software-architecture
description: Core software design principles (KISS, DRY, SOLID) and testability checklist — ALWAYS load this before planning and before writing any implementation code
compatibility: opencode
---

## Mandatory usage

Apply this skill:

- **Before planning** any feature, module, or system change.
- **While building**, at every significant design decision (new type, new module, new abstraction, new interface).

---

## KISS — Keep It Simple

- Always prefer the simplest solution that correctly solves the problem.
- Reject unnecessary abstraction layers, extra indirection, and over-engineering.
- If a design is difficult to explain in one sentence, it is too complex — simplify it first.
- Prefer flat structures over deeply nested ones.
- Avoid premature optimization. Measure before you optimize.

---

## DRY — Don't Repeat Yourself

- Every piece of knowledge or logic must have a single, authoritative representation.
- Before writing new code, search the codebase for existing utilities, helpers, or patterns that already solve the problem (use `grep`, `glob`, or the `explore` subagent).
- Extract shared logic into reusable, well-named units (functions, classes, modules).
- Duplication in tests is acceptable when it improves clarity. Duplication in production logic is not.

---

## SOLID

Apply all five principles:

1. **Single Responsibility (SRP):** Each class, module, or function does exactly one thing. If you need "and" to describe it, split it.
2. **Open/Closed (OCP):** Design for extension without modifying existing code. Prefer composition and interfaces over large inheritance trees and conditional branches.
3. **Liskov Substitution (LSP):** Subtypes must be substitutable for their base types without altering correctness or invariants.
4. **Interface Segregation (ISP):** Prefer narrow, focused interfaces. Callers must not depend on methods they do not use.
5. **Dependency Inversion (DIP):** High-level modules must not depend on low-level modules. Both should depend on abstractions (interfaces, traits, abstract classes).

---

## Testability

Before finalizing any design, explicitly answer these questions:

- **Can this be unit-tested in isolation?** If not, introduce an abstraction (interface / dependency injection) to decouple it from I/O, time, randomness, or external state.
- **What are the boundaries?** Identify and isolate all I/O, time, randomness, and external dependencies — they must be injectable or mockable.
- **What are the failure modes?** Design so that error paths are as testable as the happy path.
- **Is the public API minimal and stable?** Smaller surfaces are easier to test and harder to break accidentally.

---

## Checklist

Run this mentally before every plan and every implementation step:

- [ ] Is this the simplest design that works? (KISS)
- [ ] Am I duplicating logic that already exists in this codebase? (DRY)
- [ ] Does each component have a single, clear responsibility? (SRP)
- [ ] Can I extend this later without modifying existing code? (OCP)
- [ ] Are dependencies pointing toward abstractions, not concretions? (DIP)
- [ ] Can I write a focused unit test for this without spinning up the full system? (Testability)
- [ ] Have I identified the error paths and can they be tested too? (Testability)
