#pragma once

#include "bzd/container/span.h"
#include "bzd/container/storage/non_owning.h"
#include "bzd/platform/types.h"

namespace bzd::impl {
template <class T>
class StringView : public impl::Span<T, impl::NonOwningStorage<T>>
{
protected:
	using Parent = impl::Span<T, impl::NonOwningStorage<T>>;
	using StorageType = typename Parent::StorageType;

protected:
	// This function is not constepr, hence need to re-implement it
	constexpr SizeType strlen(const T* const str) noexcept
	{
		SizeType length = 0;
		while (str[length] != '\0')
		{
			++length;
		}
		return length;
	}

public:
	constexpr StringView() noexcept : Parent{StorageType{nullptr, 0}} {}
	constexpr StringView(const T* const str) noexcept : Parent{StorageType{str, strlen(str)}} {}
	template <SizeType N>
	constexpr StringView(const T (&data)[N]) noexcept : Parent{StorageType{data, N}}
	{
	}
	constexpr StringView(const T* const str, const SizeType size) noexcept : Parent{StorageType{str, size}} {}
	constexpr StringView(const bzd::Span<char>& span) noexcept : Parent{StorageType{span.data(), span.size()}} {}

	constexpr StringView subStr(const SizeType pos, const SizeType count = StringView::npos) const noexcept
	{
		return StringView(&this->at(pos), (count == StringView::npos) ? (this->size() - pos) : count);
	}

	constexpr void removePrefix(const SizeType n) noexcept
	{
		this->storage_.dataMutable() += n;
		this->storage_.sizeMutable() -= n;
	}

	constexpr void clear() noexcept { removePrefix(this->size()); }
};
} // namespace bzd::impl

namespace bzd {
using StringView = impl::StringView<const char>;
} // namespace bzd

constexpr bzd::StringView operator""_sv(const char* str, bzd::SizeType size) noexcept
{
	return bzd::StringView{str, size};
}

#define CSTR(strLiteral)                                                                                  \
	[] {                                                                                                  \
		using StrView = struct                                                                            \
		{                                                                                                 \
			static constexpr auto value() { return bzd::StringView(strLiteral, sizeof(strLiteral) - 1); } \
		};                                                                                                \
		return StrView{};                                                                                 \
	}()
