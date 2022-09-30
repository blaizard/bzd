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

constexpr void toString(bzd::interface::String& str, const bzd::interface::String& data) noexcept { str.append(data); }

constexpr void toString(bzd::interface::String& str, const bzd::StringView data) noexcept { str.append(data); }

constexpr void toString(bzd::interface::String& str, const char* const data) noexcept { str.append(bzd::StringView{data}); }
