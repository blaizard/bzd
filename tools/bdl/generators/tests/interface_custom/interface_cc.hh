#pragma once

#include "cc/bzd/platform/types.hh"
#include "tools/bdl/generators/cc/adapter/types.hh"

namespace bzd::test {

template <class Impl>
class MyInterface
{
public:
	bzd::Int32Type add(const bzd::Int32Type& a, const bzd::Int32Type& b) noexcept
	{
		return bzd::impl::getMethod(this, &MyInterface::add, &Impl::add)->add(a, b);
	}
};

} // namespace bzd::test
