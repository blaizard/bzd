#pragma once

#include "cc/bzd/algorithm/byte_copy.hh"
#include "cc/bzd/algorithm/copy.hh"
#include "cc/bzd/algorithm/fill_n.hh"
#include "cc/bzd/container/impl/span_resizeable.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/storage/non_owning.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/meta/string_literal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/add_const.hh"
#include "cc/bzd/type_traits/is_trivially_copyable.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/bit/endian.hh"
#include "cc/bzd/utility/min.hh"

namespace bzd::impl {

struct StringPolicies
{
	static constexpr void post(auto& self) { self.at(self.size()) = '\0'; }
};

template <class T, class Storage>
class String : public impl::SpanResizeable<T, Storage, StringPolicies>
{
public:
	using Self = String;
	using Parent = impl::SpanResizeable<T, Storage, StringPolicies>;
	using StorageType = typename Parent::StorageType;
	using ValueType = typename Parent::ValueType;
	using StringView = bzd::impl::StringView<bzd::typeTraits::AddConst<T>>;

public:
	constexpr String(const Storage& storage, const bzd::Size capacity) noexcept : Parent{storage, capacity - 1u} {}

	// Copy/move constructor/assignment.
	constexpr String(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr String(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = delete;

	// Append
	template <Size n>
	constexpr Size append(const T (&str)[n]) noexcept
	{
		return append(bzd::StringView{str});
	}
	constexpr Size append(const T* const str) noexcept { return append(bzd::StringView{str}); }
	template <class... Args>
	constexpr Size append(Args&&... args) noexcept
	{
		return Parent::append(bzd::forward<Args>(args)...);
	}

	// Assign
	template <Size n>
	constexpr Size assign(const T (&str)[n]) noexcept
	{
		return assign(bzd::StringView{str});
	}
	constexpr Size assign(const T* const str) noexcept { return assign(bzd::StringView{str}); }
	template <class... Args>
	constexpr Size assign(Args&&... args) noexcept
	{
		return Parent::assign(bzd::forward<Args>(args)...);
	}

	template <class U>
	constexpr Self& operator+=(U&& data) noexcept
	{
		append(bzd::forward<U>(data));
		return *this;
	}

	template <class U>
	constexpr Self& operator=(U&& data) noexcept
	{
		assign(bzd::forward<U>(data));
		return *this;
	}
};
} // namespace bzd::impl

namespace bzd::interface {
using String = impl::String<char, impl::NonOwningStorage<char>>;
}

namespace bzd {

template <Size maxCapacity>
class String : public interface::String
{
public:
	using Self = String;
	using Parent = interface::String;
	using StorageType = typename Parent::StorageType;
	using ValueType = typename interface::String::ValueType;

public:
	constexpr String() noexcept : Parent{StorageType{data_, 0}, maxCapacity + 1u} { data_[0] = '\0'; }
	constexpr String(const bzd::StringView& str) noexcept : String() { this->append(str); }

	// Copy/move constructor/assignment.
	String(const Self&) = default;
	Self& operator=(const Self&) = delete;
	String(Self&&) = default;
	Self& operator=(Self&&) = delete;

public:
	template <class T>
	constexpr Self& operator=(const T& data) noexcept
	{
		this->assign(data);
		return *this;
	}

	constexpr operator bzd::UInt64() const noexcept
	requires(maxCapacity < 8u)
	{
		bzd::UInt64 value{0u};
		// This should be optimized away by the compiler.
		for (bzd::Size i = 0u; i < maxCapacity; ++i)
		{
			if constexpr (bzd::Endian::native == bzd::Endian::little)
			{
				value += static_cast<bzd::UInt64>(data_[i]) << (i * 8u);
			}
			else
			{
				value += static_cast<bzd::UInt64>(data_[maxCapacity - i - 1u]) << (i * 8u);
			}
		}
		return value;
	}

protected:
	alignas(alignof(bzd::UInt64)) ValueType data_[maxCapacity + 1u]{}; // needed for constexpr
};

// Comparison to char*
constexpr bool operator==(const interface::String& lhs, const char* const rhs) noexcept
{
	return (StringView{lhs.data(), lhs.size()} == StringView{rhs});
}

} // namespace bzd

template <bzd::meta::StringLiteral str>
constexpr bzd::String<str.size()> operator""_s() noexcept
{
	return {str.data()};
}
