#pragma once

#include "cc/bzd/algorithm/copy.hh"
#include "cc/bzd/container/impl/span.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/storage/non_owning.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/add_const.hh"
#include "cc/bzd/utility/min.hh"

namespace bzd::impl {
template <class T, class Storage>
class String : public impl::Span<T, Storage>
{
protected:
	using Self = String;
	using Parent = impl::Span<T, Storage>;
	using StorageType = typename Parent::StorageType;
	using ValueType = typename Parent::ValueType;
	using StringView = bzd::impl::StringView<bzd::typeTraits::AddConst<T>>;

public:
	constexpr String(const Storage& storage, const bzd::SizeType capacity) noexcept : Parent{Storage{storage}}, capacity_{capacity} {}

	// Copy/move constructor/assignment.
	constexpr String(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr String(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = delete;

	// Converter
	constexpr SizeType append(const bzd::StringView& str) noexcept { return append(str.data(), str.size()); }
	constexpr SizeType append(const Self& str) noexcept { return append(str.data(), str.size()); }
	constexpr SizeType append(const T c) noexcept { return append(&c, 1); }
	constexpr SizeType append(const T* data, const SizeType n) noexcept
	{
		// Handles overflows
		const SizeType sizeLeft = capacity_ - this->size() - 1;
		const SizeType actualN = bzd::min(sizeLeft, n);
		bzd::algorithm::copy(data, data + actualN, &this->at(this->size()));
		this->storage_.sizeMutable() += actualN;
		this->at(this->size()) = '\0';

		return actualN;
	}

	// Fill
	constexpr SizeType append(const SizeType n, const T c) noexcept
	{
		const SizeType sizeLeft = capacity_ - this->size() - 1;
		const SizeType actualN = bzd::min(sizeLeft, n);
		for (SizeType i = 0; i < actualN; ++i)
		{
			this->at(this->size() + i) = c;
		}
		this->storage_.sizeMutable() += actualN;
		this->at(this->size()) = '\0';

		return actualN;
	}

	template <class U>
	constexpr SizeType assign(const U& data) noexcept
	{
		clear();
		return append(data);
	}

	constexpr SizeType capacity() const noexcept { return capacity_ - 1; }

	constexpr void clear() noexcept { resize(0); }

	constexpr void resize(const SizeType n) noexcept
	{
		this->storage_.sizeMutable() = (n < capacity_ - 1) ? n : capacity_ - 1;
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

public:
	const bzd::SizeType capacity_{};
};
} // namespace bzd::impl

namespace bzd::interface {
using String = impl::String<char, impl::NonOwningStorage<char>>;
}

namespace bzd {
template <SizeType N>
class String : public interface::String
{
protected:
	using Self = String;
	using Parent = interface::String;
	using StorageType = typename Parent::StorageType;
	using ValueType = typename interface::String::ValueType;

public:
	constexpr String() noexcept : Parent{StorageType{data_, 0}, N + 1} { data_[0] = '\0'; }
	constexpr String(const bzd::StringView& str) noexcept : String() { append(str); }
	constexpr String(const SizeType n, const char c) noexcept : String() { append(n, c); }

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
	ValueType data_[N + 1]{}; // needed for constexpr
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
