---
name: sanitize
description: Sanitizes, refactors, and cleans up modified code files to meet senior engineering standards. Use when the user asks to sanitize, clean up, refactor code, run formatters/linters, fix comments, or prepare files for a commit.
compatibility: opencode
---

Act as a Senior Software Engineer and sanitize the changes you are working on by:

- Adding clear documentation for each public function/API.
- Ensuring existing comments still make sense and are grammatically correct.
- Making symbol names descriptive and convention-compliant.
- Removing dead code, unused imports/variables, and commented-out block.
- Replacing magic numbers/unclear literals with named constants where appropriate.
- Reviewing the commit message and proposing an improved version if needed.
- Running the standard formatter, linter, and/or pre-commit hooks for this codebase.
