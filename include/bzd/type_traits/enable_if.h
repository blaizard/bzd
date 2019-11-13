#pragma once

namespace bzd
{
	namespace typeTraits
	{
		template <bool B, class T = void>
		struct enableIf {};
	
		template <class T>
		struct enableIf<true, T> { typedef T type; };
	}
}
