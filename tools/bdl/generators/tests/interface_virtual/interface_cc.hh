#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::test {

class MyInterface
{
public:
	virtual bzd::Int32 add(const Int32& a, const Int32& b) noexcept = 0;
};

} // namespace bzd::test
