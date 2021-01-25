#pragma once

#include "bzd/container/iterator/contiguous.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"
#include "bzd/type_traits/enable_if.h"
#include "bzd/type_traits/is_const.h"
#include "bzd/type_traits/remove_const.h"
#include "bzd/utility/swap.h"

namespace bzd {
/**
 * \brief The class template span describes an object that can refer to a
 * contiguous sequence of objects with the first element of the sequence at
 * position zero.
 *
 * \tparam T Element type, must be a complete type that is not an abstract class
 * type.
 */
template <class T>
class Span
{
protected:
	using DataType = T;
	using SelfType = Span<DataType>;
	using IsDataConst = bzd::typeTraits::IsConst<DataType>;

public:
	using Iterator = bzd::iterator::Contiguous<DataType>;

	static constexpr const SizeType npos = static_cast<SizeType>(-1);

public:
	constexpr Span() noexcept = default;
	constexpr Span(DataType* const data, const SizeType size) noexcept : data_(data), size_(size) {}

	template <bzd::SizeType N>
	constexpr Span(DataType (&data)[N]) noexcept : data_(data), size_(N)
	{
	}

	template <class Q = IsDataConst, bzd::typeTraits::EnableIf<Q::value, void>* = nullptr>
	constexpr Span(const Span<bzd::typeTraits::RemoveConst<DataType>>& span) noexcept : data_(span.data_), size_(span.size_)
	{
	}

	// Iterators

	constexpr Iterator begin() const noexcept { return Iterator(data_, 0); }

	constexpr Iterator end() const noexcept { return Iterator(data_, size()); }

	// Size

	constexpr SizeType size() const noexcept { return size_; }

	constexpr SizeType sizeBytes() const noexcept { return size_ * sizeof(DataType); }

	constexpr bool empty() const noexcept { return (size_ == 0); }

	// Equality operator

	constexpr bool operator==(const SelfType& rhs) const noexcept
	{
		if (size() != rhs.size())
		{
			return false;
		}

		for (bzd::SizeType index = 0; index < size(); ++index)
		{
			if (at(index) != rhs.at(index))
			{
				return false;
			}
		}

		return true;
	}

	// Inequality operator

	constexpr bool operator!=(const SelfType& rhs) const noexcept { return !(*this == rhs); }

	// Accessors

	constexpr DataType& operator[](const SizeType index) const noexcept { return at(index); }

	// at

	constexpr DataType& at(const SizeType index) const noexcept { return data_[index]; }

	// front

	constexpr DataType& front() const noexcept { return at(0); }

	// back

	constexpr DataType& back() const noexcept { return at(size_ - 1); }

	// data

	constexpr DataType* data() const noexcept { return data_; }

	// find

	constexpr SizeType find(const DataType& item, const SizeType start = 0) const noexcept
	{
		for (SizeType i = start; i < size_; ++i)
		{
			if (at(i) == item)
			{
				return i;
			}
		}
		return npos;
	}

	// subviews

	constexpr Span<DataType> subspan(const SizeType offset = 0, const SizeType count = npos) const noexcept
	{
		bzd::assert::isTrue(offset < size_);
		const auto actualCount = (count == npos) ? (size_ - offset) : count;
		bzd::assert::isTrue(offset + actualCount <= size_);
		return Span<DataType>{data_ + offset, actualCount};
	}

	constexpr Span<DataType> first(const SizeType count) const noexcept { return subspan(/*offset*/ 0, count); }

	constexpr Span<DataType> last(const SizeType count) const noexcept { return subspan(/*offset*/ size_ - count, count); }

	// Convert to bytes

	constexpr Span<const bzd::ByteType> asBytes() const noexcept
	{
		return Span<const bzd::ByteType>{reinterpret_cast<const bzd::ByteType*>(data_), sizeBytes()};
	}

	constexpr Span<bzd::ByteType> asWritableBytes() const noexcept
	{
		return Span<bzd::ByteType>{reinterpret_cast<bzd::ByteType*>(data_), sizeBytes()};
	}

protected:
	template <class Q>
	friend class Span;

	DataType* data_ = nullptr;
	SizeType size_ = 0;
};
} // namespace bzd
