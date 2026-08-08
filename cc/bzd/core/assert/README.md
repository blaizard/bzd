# Assert

Runtime assertions that log an error and trigger a platform panic when a condition is not met.

## Usage

```c++
bzd::assert::isTrue(size > 0, "Invalid size: {}"_csv, size);
bzd::assert::isResult(result);
bzd::assert::unreachable();
```

Assertions automatically include the source location (file and line) in the reported error.
