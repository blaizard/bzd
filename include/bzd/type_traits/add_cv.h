#pragma once

namespace bzd
{
	namespace typeTraits
	{
		template <class T> struct addCV { typedef const volatile T type; };
	}
}
