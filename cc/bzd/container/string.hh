#pragma once

#include "cc/bzd/algorithm/copy.hh"
#include "cc/bzd/algorithm/fill_n.hh"
#include "cc/bzd/container/impl/span_resizeable.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/storage/non_owning.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/add_const.hh"
#include "cc/bzd/type_traits/is_trivially_copyable.hh"
#include "cc/bzd/utility/min.hh"

namespace bzd::impl {
template <class T, class Storage>
class String : public impl::SpanResizeable<T, Storage>
{
protected:
	using Self = String;
	using Parent = impl::SpanResizeable<T, Storage>;
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
	constexpr Size append(const bzd::StringView& str) noexcept { return pushBack(str); }
	constexpr Size append(const bzd::impl::String<T, Storage>& str) noexcept { return pushBack(str); }
	constexpr Size append(const T c) noexcept { return pushBack(c); }

	template <class... Args>
	constexpr Size pushBack(Args&&... args) noexcept
	{
		const auto count = Parent::pushBack(bzd::forward<Args>(args)...);
		this->at(this->size()) = '\0';
		return count;
	}

	template <class U>
	constexpr Size assign(const U& data) noexcept
	{
		clear();
		return append(data);
	}

	constexpr void clear() noexcept
	{
		Parent::clear();
		this->at(0) = '\0';
	}

	constexpr void resize(const Size n) noexcept
	{
		Parent::resize(n);
		this->at(this->size()) = '\0';
	}

	template <class U>
	constexpr Self& operator+=(const U& data) noexcept
	{
		append(data);
		return *this;
	}

	template <class U>
	constexpr Self& operator=(const U& data) noexcept
	{
		assign(data);
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
protected:
	using Self = String;
	using Parent = interface::String;
	using StorageType = typename Parent::StorageType;
	using ValueType = typename interface::String::ValueType;

public:
	constexpr String() noexcept : Parent{StorageType{data_, 0}, maxCapacity + 1u} { data_[0] = '\0'; }
	constexpr String(const bzd::StringView& str) noexcept : String() { append(str); }

	template <class T>
	constexpr Self& operator=(const T& data) noexcept
	{
		assign(data);
		return *this;
	}

	// Copy/move constructor/assignment.
	constexpr String(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr String(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = delete;

protected:
	ValueType data_[maxCapacity + 1u]{}; // needed for constexpr
};

// Comparison to char*
constexpr bool operator==(const interface::String& lhs, const char* const rhs) noexcept
{
	return (StringView{lhs.data(), lhs.size()} == StringView{rhs});
}

} // namespace bzd

constexpr void toString(bzd::interface::String& str, const bzd::interface::String& data) noexcept
{
	str.append(data);
}

constexpr void toString(bzd::interface::String& str, const bzd::StringView data) noexcept
{
	str.append(data);
}

constexpr void toString(bzd::interface::String& str, const char* const data) noexcept
{
	str.append(bzd::StringView{data});
}
