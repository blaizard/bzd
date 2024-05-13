#pragma once

#include "cc/bdl/generator/impl/adapter/types.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::test {

template <class Impl>
class MyInterface
{
public:
	auto add(const bzd::Int32& a, const bzd::Int32& b) noexcept { return bzd::impl::getImplementation(this)->add(a, b); }
};

} // namespace bzd::test
