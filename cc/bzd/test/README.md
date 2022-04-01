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
