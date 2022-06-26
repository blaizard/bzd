#pragma once

namespace bzd::test {

template <class Impl>
class MyInterface
{
public:
    bzd::Int32Type add(const Int32Type& a, const Int32Type& b) noexcept
    {
        return bzd::impl::getMethod(this, &MyInterface::add, &Impl::add)->add(a, b);
    }
};

}
