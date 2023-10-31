# Form

## Input with context

When subscribing to an element, we can receive its `value` on change but also its `context`.

The context contains the following data dependince on the update action:

All the common attributes are available:

- `name`: The name of the element that is modified.
- `action`: The type of action that triggered the event.
- `row`: For `Array` and `Table` elements type, the index of the row that was affected.

In addition, depending on the action value, the following extra attributes may apply:

- `update`: When a value has been changed.
  - none
- `add`: A new element was added.
  - `value`: The new element that has been added.
- `delete`: An element is removed.
  - `value`: The value of the element that has been removed.
- `move`:
  - `rowFrom`: The original index of the element.
  - `rowTo`: The new index of the element.
  - `value`: The value of the element that has been moved.

Note: all the `value` fields here are internal values, not external. In otherword, are the values transformed by the `toInputValue` option, if applicable.
