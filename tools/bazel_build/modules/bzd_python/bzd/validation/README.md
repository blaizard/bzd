# Validation

Framework to validate dictionaries.

## Usage

The simplest usage form is as follow:

```py
# Create a validation schema, here it tests that the entry "test" validates into an integer
schema = Validation({"test": "integer"})
# Test the schema on a dictionary, a result is returned.
# If the schema does not validate, it throws (by default, this behavior can be configured)
result = schema.validate({"test": "2"})
# The result can be used to access the parsed values
result.values == {"test": 2}
# The result can also be evaluated to bool to check if the result is valid. Note that in this
# example, this is irrelevant as the validate function is configured to throw by default.
```

It also works similarly for lists:

```py
# Create a validation schema, here it tests that a list has one element and is an integer.
schema = Validation(["integer"])
result = schema.validate(["2"])
# The result can be used to access the parsed values
result.values == [2]
```

### Noexcept

The validate function can be used to not throw if the schema does not validate. This is done as follow:

```py
result = schema.validate({"test": "2"}, output="return")
if not result:
    # Handle error here!
```

## Constraints

The following constraints are available:

| Name      | Extends        | Underlying | Documentation                                     |
| --------- | -------------- | ---------- | ------------------------------------------------- |
| boolean   |                | bool       | Check that it represents a boolean (true/false).  |
| float     | min(i), max(i) | float      | Check that it represents a floating point number. |
| integer   | min(i), max(i) | int        | Check that it represents an integer number.       |
| mandatory |                |            | Ensure that the entry is present.                 |
| string    | min(i), max(i) | str        | Check that it represents a string.                |

### Custom constraints

It is possible to enrich the validation by adding custom constraints.
The new constraint must inherit from `Constraint`.

The validation process is performed in 2 different steps:

1. `install` the constraint. This step checks the format on the constraint itself, for example,
   if we use `min` constraint, it ensures that it contains a single integer argument: `min(12)` for example.
   This step is executed during the object construction phase.
2. `check` the constraint. This will do check on the value itself, therefore this is performed during the
   `validate` function call. In addition, during this step, the user can perform the following operations: - Introduce extra temporary constraint to this entry, that will be validated afterward. - Set the underlying value, this can be accessed from the result.
