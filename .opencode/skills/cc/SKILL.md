---
name: cc
description: How to write C++ code
compatibility: opencode
---

Always enforce strict modern C++ practices:

- **Const correctness:** Mark non-mutating variables, references, and member functions `const`.
- **Noexcept:** Mark functions `noexcept` when guaranteed not to throw.
- **Constexpr:** Use `constexpr` (or `consteval`) for expressions evaluable at compile time.
- **Final & Override:** Use `override` on virtual function overrides and `final` on non-overridable classes/virtuals.
- **Explicit:** Mark single-argument constructors and conversion operators `explicit`.
- **Nodiscard:** Annotate functions with `[[nodiscard]]` where ignoring the return value is a bug.
