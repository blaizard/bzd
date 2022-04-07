#pragma once

#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/storage/non_owning.hh"
#include "cc/bzd/meta/range.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/utility/concept.hh"

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

	constexpr StringView subStr(const SizeType pos, const SizeType count = npos) const noexcept
	{
		return StringView(&this->at(pos), (count == npos) ? (this->size() - pos) : count);
	}

	constexpr void removePrefix(const SizeType n) noexcept
	{
		this->storage_.dataMutable() += n;
		this->storage_.sizeMutable() -= n;
	}

	constexpr void clear() noexcept { removePrefix(this->size()); }
};

/// Type used by the compile time string view type.
template <bzd::SizeType n>
struct ConstexprStringViewArray
{
	template <bzd::SizeType... i>
	constexpr ConstexprStringViewArray(const char (&arr)[n], bzd::meta::range::Type<i...>) noexcept : data{arr[i]..., '\0'}
	{
	}
	constexpr ConstexprStringViewArray(char const (&arr)[n]) noexcept : ConstexprStringViewArray(arr, bzd::meta::Range<0, n>{}) {}

	const char data[n + 1];
};

} // namespace bzd::impl

namespace bzd {
using StringView = impl::StringView<const char>;

/// Base class for constant StringView from operator""_csv
struct ConstexprStringView
{
};

} // namespace bzd

namespace bzd::concepts {

template <class T>
concept constexprStringView = typeTraits::isBaseOf<ConstexprStringView, T>;

}

/// Operator to create a string view out of a string
constexpr bzd::StringView operator""_sv(const char* str, bzd::SizeType size) noexcept
{
	return bzd::StringView{str, size};
}

/// Operator to create a compile time string view out of a string.
template <bzd::impl::ConstexprStringViewArray str>
constexpr auto operator""_csv() noexcept
{
	struct StrView : ::bzd::ConstexprStringView
	{
		[[nodiscard]] static constexpr const char* value() noexcept { return str.data; }
	};
	return StrView{};
}
