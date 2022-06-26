#pragma once

namespace bzd::test {

class MyInterface
{
public:
    virtual bzd::Int32Type add(const Int32Type& a, const Int32Type& b) noexcept = 0;
};

}
