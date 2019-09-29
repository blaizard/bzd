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
		protected:
			using Impl::size_;
			using Impl::data_;

		public:
			constexpr StringView(const T* const str) : Impl(str, strlen(str)) {}
			constexpr StringView(const T* const str, const SizeType size) : Impl(str, size) {}

			constexpr StringView substr(const SizeType pos, const SizeType count = StringView::npos) const noexcept
			{
				return StringView(&data_[pos], (count == StringView::npos) ? (size_ - pos) : count);
			}

			constexpr void removePrefix(const SizeType n) noexcept
			{
				data_ += n;
				size_ -= n;
			}
		};
	}

	using StringView = impl::StringView<char, ConstSpan<char>>;
}
