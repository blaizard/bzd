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
	using IsConst = bzd::typeTraits::IsConst<DataType>;

public:
	using Iterator = bzd::iterator::Contiguous<DataType>;
	using ConstIterator = bzd::iterator::Contiguous<const DataType>;

	static constexpr const SizeType npos = static_cast<SizeType>(-1);

public:
	constexpr Span() noexcept = default;
	constexpr Span(DataType* const data, const SizeType size) noexcept : data_(data), size_(size) {}

	template <class Q = IsConst, bzd::typeTraits::EnableIf<Q::value, void>* = nullptr>
	constexpr Span(const Span<bzd::typeTraits::RemoveConst<DataType>>& span) noexcept : data_(span.data_), size_(span.size_)
	{
	}

	template <class Q = IsConst, bzd::typeTraits::EnableIf<!Q::value, void>* = nullptr>
	constexpr Iterator begin() noexcept
	{
		return Iterator(data_, 0);
	}

	template <class Q = IsConst, bzd::typeTraits::EnableIf<!Q::value, void>* = nullptr>
	constexpr Iterator end() noexcept
	{
		return Iterator(data_, size());
	}

	constexpr ConstIterator begin() const noexcept { return ConstIterator(data_, 0); }

	constexpr ConstIterator cbegin() const noexcept { return begin(); }

	constexpr ConstIterator end() const noexcept { return ConstIterator(data_, size()); }

	constexpr ConstIterator cend() const noexcept { return end(); }

	constexpr SizeType size() const noexcept { return size_; }

	// Equality operator

	constexpr bool operator==(const SelfType& rhs) const
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

	constexpr bool operator!=(const SelfType& rhs) const { return !(*this == rhs); }

	template <class Q = IsConst, bzd::typeTraits::EnableIf<!Q::value, void>* = nullptr>
	constexpr DataType& operator[](const SizeType index)
	{
		return data_[index];
	}

	constexpr const DataType& operator[](const SizeType index) const { return data_[index]; }

	// at

	template <class Q = IsConst, bzd::typeTraits::EnableIf<!Q::value, void>* = nullptr>
	constexpr DataType& at(const SizeType index)
	{
		return data_[index];
	}

	constexpr const T& at(const SizeType index) const { return data_[index]; }

	// front

	template <class Q = IsConst, bzd::typeTraits::EnableIf<!Q::value, void>* = nullptr>
	constexpr DataType& front() noexcept
	{
		return data_[0];
	}
	constexpr const DataType& front() const noexcept { return data_[0]; }

	// back

	template <class Q = IsConst, bzd::typeTraits::EnableIf<!Q::value, void>* = nullptr>
	constexpr DataType& back() noexcept
	{
		return data_[size_ - 1];
	}
	constexpr const DataType& back() const noexcept { return data_[size_ - 1]; }

	template <class Q = IsConst, bzd::typeTraits::EnableIf<!Q::value, void>* = nullptr>
	constexpr DataType* data() noexcept
	{
		return data_;
	}

	constexpr const DataType* data() const noexcept { return data_; }

	constexpr SizeType find(const DataType& item, const SizeType start = 0) const noexcept
	{
		for (SizeType i = start; i < size_; ++i)
		{
			if (data_[i] == item)
			{
				return i;
			}
		}
		return npos;
	}

	constexpr bool empty() const noexcept { return (size_ == 0); }

protected:
	template <class Q>
	friend class Span;

	T* data_ = nullptr;
	SizeType size_ = 0;
};
} // namespace bzd
