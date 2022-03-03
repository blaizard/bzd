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
