#pragma once

namespace bzd { namespace typeTraits {
template <class T>
struct addVolatile
{
	typedef volatile T type;
};
}}	// namespace bzd::typeTraits
