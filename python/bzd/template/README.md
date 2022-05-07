# Template

Bzd templating language (`btl`) library that supports:

Syntax:

- Substitutions blocks are denoted as follow: `{{...}}`
- Control blocks are denoted as follow: `{%...%}`
- Comment blocks are denoted as follow: `{#...#}`

- White spaces:

  - All blocks implement whitespace control. Using `-` inside a block will strip the left of right side of the expression,
    ex: `{%- ... %}` or `{{ ... -}}`
  - The first newline after a block and trailing spaces are stripped (for all control and comment blocks).

- Substitutions:

  - Direct and nested, ex: `{{ my.nested.var }}`.
  - Resolution from mapping or sequences.
  - Support piping to a callable, ex: `{{ var | lower | capitalize }}`.
  - Support argument injection, ex: `{{ callable(my.var, 12, "hello", false) }}`
  - Support arrays and nested arrays, ex: `{{ a[b.c[d]].u }}`
  - Support a combination of all of this.

Piped data can be of arbitrary type but the final substitued value must be either a number or a string.

- For loops:

  - Simple iterable, ex: `{% for value in data %} ... {% end %}`.
  - Key value and index-based iterable, ex: `{% for value in data %} ... {% end %}`.
  - Index-based iterable, ex: `{% for index, data in my.list %} ... {% end %}`.

- If blocks:

  - Bool evaluation of condition, ex: `{% if value %} ... {% end %}`
  - Comparison operators (`==`, `!=`, `<`, `>`, `>=`, `<=`), ex: `{% if value == 12 %} ... {% end %}`
  - Negation operator (`not`), ex: `{% if not value %} ... {% end %}`
  - Association operators (`and`, `or`), ex: `{% if value == 12 or value == 14 %} ... {% end %}`
  - In operators, ex: `{% if value in [1, 2, 3] %} ... {% end %}`
  - Consecutive `else` and `elif` statement, ex: `{% if value1 %} ... {% elif value2 %} ... {% else %} ... {% end %}`

- Macro blocks:

  - Reusable block anywhere, ex: `{% macro my_block() %} ... {% end %}` -> `{{ my_block() }}`
  - Symbol substitution, ex: `{% macro my_block(a) %} ... {% end %}` -> `{{ my_block(b) }}`

- Inclusion blocks:

  - Include another btl file as if it was part of the document, ex: `{% include "my/path.btl" %}`
