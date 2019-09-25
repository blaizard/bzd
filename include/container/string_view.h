#pragma once

#include "include/types.h"
#include "include/container/span.h"

namespace bzd
{
	namespace impl
	{
		template <class T, class Impl>
		class StringView : public Impl
		{
		public:
			StringView(const T* const str) : Impl(str, strlen(str)) {}
			StringView(const T* const str, const SizeType size) : Impl(str, size) {}
		};
	}

	using StringView = impl::StringView<char, Span<char, /*IsDataConst*/true>>;
}
