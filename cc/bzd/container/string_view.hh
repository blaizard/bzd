#pragma once

#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/storage/non_owning.hh"
#include "cc/bzd/meta/string_literal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/min.hh"

namespace bzd::impl {
template <class T>
class StringView : public impl::Span<T, impl::NonOwningStorage<T>>
{
protected:
	using Self = StringView<T>;
	using Parent = impl::Span<T, impl::NonOwningStorage<T>>;
	using StorageType = typename Parent::StorageType;

protected:
	// This function is not constepr, hence need to re-implement it
	constexpr Size strlen(const T* const str) noexcept
	{
		Size length = 0;
		while (str[length] != '\0')
		{
			++length;
		}
		return length;
	}

public:
	constexpr StringView() noexcept : Parent{StorageType{nullptr, 0}} {}
	constexpr StringView(const T* const str) noexcept : Parent{StorageType{str, strlen(str)}} {}
	template <Size N>
	constexpr StringView(const T (&data)[N]) noexcept : Parent{StorageType{data, N}}
	{
	}
	constexpr StringView(const T* const str, const Size size) noexcept : Parent{StorageType{str, size}} {}
	constexpr StringView(const bzd::Span<char>& span) noexcept : Parent{StorageType{span.data(), span.size()}} {}

	constexpr StringView subStr(const Size pos, const Size count = npos) const noexcept
	{
		const auto maxSize = this->size() - pos;
		return StringView(&this->at(pos), (count == npos) ? maxSize : bzd::min(maxSize, count));
	}

	constexpr void removePrefix(const Size n) noexcept { this->storage_.removePrefix(n); }

	constexpr void clear() noexcept { removePrefix(this->size()); }
};

} // namespace bzd::impl

namespace bzd {
using StringView = impl::StringView<const char>;

/// Base class for constant StringView from operator""_csv
struct ConstexprStringView
{
};

} // namespace bzd

namespace bzd::typeTraits {
template <>
inline constexpr bzd::Bool enableBorrowedRange<bzd::StringView> = true;
}

namespace bzd::concepts {

template <class T>
concept constexprStringView = typeTraits::isBaseOf<ConstexprStringView, T>;

}

/// Operator to create a string view out of a string
constexpr bzd::StringView operator""_sv(const char* str, bzd::Size size) noexcept { return bzd::StringView{str, size}; }

/// Operator to create a compile time string view out of a string.
template <bzd::meta::StringLiteral str>
constexpr auto operator""_csv() noexcept
{
	struct ConstStrView : ::bzd::ConstexprStringView
	{
		[[nodiscard]] static constexpr const char* value() noexcept { return str.data(); }
	};
	return ConstStrView{};
}
