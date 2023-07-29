# Ranges

Ranges are any container with `begin()` and `end()` methods that return respectively an iterator and a sentinel to this iterator.

Range can be owning or non-owning.

Views is a subset of ranges that do not own the underlying data like a range might do. It only points to it through its iterator pair.

A view can be created from a range using `ranges::all`.

Great link to understand the `borrowed_range` concept: https://tristanbrindle.com/posts/rvalue-ranges-and-views
