#pragma once

namespace bzd { namespace typeTraits {
template <class T>
struct addConst
{
	typedef const T type;
};
}}	// namespace bzd::typeTraits
