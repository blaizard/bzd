---
name: software-architecture
description: Core software design principles — ALWAYS load this before planning and before writing any implementation code
compatibility: opencode
---

## Mandatory usage

Apply this skill:

- **Before planning** any feature, module, or system change.
- **While building**, at every significant design decision (new type, new module, new abstraction, new interface).

## KISS — Keep It Simple

- Minimalism: Prefer the simplest working solution. Flat > nested. Reject premature optimization.
- The 1-Sentence Rule: If the design cannot be explained clearly in one sentence, it is over-engineered. Simplify it.

## DRY — Don't Repeat Yourself

- Single Source of Truth: Search the codebase for existing utilities before writing new logic.
- Context Rule: Eliminate production duplication strictly. Duplication in tests is acceptable only to maximize readability.

## SOLID

- SRP: One component, one responsibility. Split it if you need the word "and" to describe its function.
- OCP & LSP: Design for extension via composition and interfaces without modifying legacy code. Subtypes must satisfy all base type invariants.
- ISP & DIP: Keep interfaces narrow and caller-focused. Depend strictly on abstractions, never on concrete implementations.

## Testability

- Isolation: Decouple code from I/O, system time, randomness, and external state using dependency injection.
- Symmetry: Public APIs must remain minimal. Design error paths to be just as testable as the happy paths.

## Self-Documenting Code

- Expressive Design: Code must speak for itself through intentional naming, clear flow, and minimal scope.
- Documentation Constraint: Avoid writing comments or external documentation unless strictly necessary to explain non-obvious business logic, or required for public, user-facing APIs.

## Checklist

Run this mentally before every plan and every implementation step:

- [ ] KISS: Is this the absolute simplest design that solves the problem?
- [ ] DRY: Have I verified this logic or pattern doesn't already exist?
- [ ] SRP: Does every new class/function have exactly one job?
- [ ] DIP: Are dependencies pointing toward interfaces/abstractions rather than concretions?
- [ ] Testability: Can I unit-test this in isolation, including all failure modes?
- [ ] Documentation: Is the code self-documenting, omitting redundant comments except for user-facing APIs?
