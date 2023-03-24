# C++ Testing Framework

## Random

The random seed is deterinistic and reproductible per test case.

A random generator factory is passed via the context of a test, using the `test` variable.

```c++
TEST(Test, Name)
{
    const auto number = test.random<int, 0, 10>();
    ...
}
```

## Friendship

Every test specialization is part of the template bzd::Template<class>, so to make a test access non-public
member of a class, just do the following:

```c++
class MyClass
{
private:
    template <class>
    friend class bzd::Test;
}

```

## Sync Barrier

The sync barrier ensures that multiple threads waits until they all reach the `barrier.wait(...)` statement.
Optionally, a callable can be called while waiting.

Usage:

```c++
#include "cc/bzd/test/sync_barrier.hh"

bzd::test::SyncBarrier barrier;

auto thread1 = [&]() {
    while (...) {
        co_await !barrier.wait(2);
    }
}

auto thread2 = [&]() {
    while (...) {
        co_await !barrier.wait(2, []() { ... });
    }
}

```
