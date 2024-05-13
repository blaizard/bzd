#pragma once

#include "cc/bdl/generator/impl/adapter/types.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::test {

template <class Impl>
class MyInterface
{
public:
	bzd::Int32 add(const bzd::Int32& a, const bzd::Int32& b) noexcept
	{
		return bzd::impl::getImplementation(this, &MyInterface::add, &Impl::add)->add(a, b);
	}
};

} // namespace bzd::test
