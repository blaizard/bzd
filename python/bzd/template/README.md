# Template

Bzd templating language (`btl`) library that supports:

## Syntax

- Substitutions blocks are denoted as follow: `{{...}}`
- Control blocks are denoted as follow: `{%...%}`
- Comment blocks are denoted as follow: `{#...#}`
- The sequences `{{`, `{%` and `{#` can respectively be escaped using: `{{{`, `{{%` and `{{#`.

### White spaces

- All blocks implement whitespace control. Using `-` inside a block will strip the left of right side of the expression,
  ex: `{%- ... %}` or `{{ ... -}}`
- The first newline after a block and trailing spaces are stripped (for all control and comment blocks).

### Substitutions

- Direct and nested, ex: `{{ my.nested.var }}`.
- Resolution from mapping or sequences.
- Support piping to a callable, ex: `{{ var | lower | capitalize }}`.
- Support argument injection, ex: `{{ callable(my.var, 12, "hello", false) }}`
- Support arrays and nested arrays, ex: `{{ a[b.c[d]].u }}`
- Support a combination of all of this.

Piped data can be of arbitrary type but the final substitued value must be either a number or a string.

### For loops

- Simple iterable, ex: `{% for value in data %} ... {% end %}`.
- Key value iterable, ex: `{% for key, value in data %} ... {% end %}`.
- Index-based iterable, ex: `{% for index, data in my.list %} ... {% end %}`.
- Builtin loop variable, ex: `{% for data in my.list %} {{ loop.index }} {{ loop.first }} {% end %}`.

### If blocks

- Bool evaluation of condition, ex: `{% if value %} ... {% end %}`
- Comparison operators (`==`, `!=`, `<`, `>`, `>=`, `<=`, `in`), ex: `{% if value == 12 %} ... {% end %}`
- Negation operator (`not`), ex: `{% if not value %} ... {% end %}`
- Consecutive `else` and `elif` statement, ex: `{% if value1 %} ... {% elif value2 %} ... {% else %} ... {% end %}`

### Macro blocks

- Reusable block anywhere, ex: `{% macro my_block() %} ... {% end %}` -> `{{ my_block() }}`
- Symbol substitution, ex: `{% macro my_block(a) %} ... {% end %}` -> `{{ my_block(b) }}`

### Inclusion blocks

- Include another btl file as if it was part of the document, ex: `{% include "my/path.btl" %}`

### Builtins utility functions

- `btl.merge`: merge different types together, ex: `btl.merge(a, b, c)`
- `btl.set`: create a set from inputs passed into arguments, ex: `btl.set(a, b, c)`
- `btl.any`: checks that any of the argument evaluates to `true`, ex: `btl.any(a, b, c)`
- `btl.all`: checks that all of the argument evaluates to `true`, ex: `btl.all(a, b, c)`
- `btl.assertTrue`: asserts that a condition evaluates to `true` otherwise print an error message and terminates, ex: `btl.assertTrue(a == 1, "'a' is not equal to 1.")`
- `btl.error`: print an unconditional error message and terminates, ex: `btl.error("This path is unreachable.")`
