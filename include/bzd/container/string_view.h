#pragma once

#include "bzd/container/span.h"
#include "bzd/types.h"

namespace bzd {
namespace impl {
template <class T, class Impl>
class StringView : public Impl
{
  protected:
	using Impl::data_;
	using Impl::size_;

	// This function is not constepr, hence need to re-implement it
	constexpr SizeType strlen(const T *const str) noexcept
	{
		SizeType length = 0;
		while (str[length] != '\0')
		{
			++length;
		}
		return length;
	}

  public:
	constexpr StringView() : Impl(nullptr, 0) {}
	constexpr StringView(const T *const str) : Impl(str, strlen(str)) {}
	constexpr StringView(const T *const str, const SizeType size) : Impl(str, size) {}
	constexpr StringView(const Span<char> &span) : Impl(span.data(), span.size()) {}

	constexpr StringView subStr(const SizeType pos, const SizeType count = StringView::npos) const noexcept
	{
		return StringView(&data_[pos], (count == StringView::npos) ? (size_ - pos) : count);
	}

	constexpr void removePrefix(const SizeType n) noexcept
	{
		data_ += n;
		size_ -= n;
	}

	constexpr void clear() noexcept { removePrefix(size_); }
};
}  // namespace impl

using StringView = impl::StringView<char, bzd::Span<const char>>;
}  // namespace bzd
