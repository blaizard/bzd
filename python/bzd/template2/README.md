# Template

Simple templating library that supports:

- Substitutions:
  - Direct and nested, ex: `{{ my.nested.var }}`.
  - Resolution from mapping, sequences or callable.
  - Support piping to a callable, ex: `{{ var | lower | capitalize }}`.

Piped data can be of arbitrary type but the final substitued value must be either a number or a string.

Substitutions are denoted as follow: `{{...}}`

- For loops:

  - Simple iterable, ex: `{% for value in data %} ... {% end %}`.
  - Key value and index-based iterable, ex: `{% for value in data %} ... {% end %}`.
  - Index-based iterable, ex: `{% for index, data in my.list %} ... {% end %}`.

- If blocks:
  - Bool evaluation of condition, ex: `{% if value %} ... {% end %}`
  - Comparison operators (`==`, `!=`, `<`, `>`, `>=`, `<=`), ex: `{% if value == 12 %} ... {% end %}`
  - Association operators (`and`, `or`), ex: `{% if value == 12 or value == 14 %} ... {% end %}`
  - In operators, ex: `{% if value in [1, 2, 3] %} ... {% end %}`
  - Consecutive `else` and `elif` statement, ex: `{% if value1 %} ... {% elif value2 %} ... {% else %} ... {% end %}`

- Macro blocks:
  - Reusable block anywhere, ex: `{% macro my_block() %} ... {% end %}` -> `{% my_block() %}`

Control blocks are denoted as follow: `{%...%}`
