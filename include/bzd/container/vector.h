#pragma once

#include "bzd/container/span.h"
#include "bzd/core/assert.h"
#include "bzd/types.h"
#include "bzd/utility/forward.h"

#include <iostream>

namespace bzd {
namespace impl {
template <class T, class Impl>
class Vector : public Impl
{
  protected:
	using Parent = Impl;
	using Impl::data_;
	using Impl::size_;

  public:
	template <class... Args>
	constexpr explicit Vector(const SizeType capacity, Args &&... args) : Impl(args...), capacity_(capacity)
	{
	}

	/**
	 * \brief Adds a new element at the end of the vector, after its current last
	 * element.
	 *
	 * The content of val is copied (or moved) to the new element.
	 *
	 * \param value Value to be copied (or moved) to the new element.
	 */
	constexpr void pushBack(T &&value)
	{
		bzd::assert::isTrue(size_ < capacity_, "Out of bound");
		Parent::at(size_) = bzd::forward<T>(value);
		++size_;
	}

	/**
	 * \brief Returns the maximum number of elements the vector can hold.
	 *
	 * \return Maximum number of element this vector can hold.
	 */
	constexpr SizeType capacity() const noexcept { return capacity_; }

	/**
	 * \brief Removes all elements.
	 */
	constexpr void clear() noexcept { resize(0); }

	/**
	 * \brief Change the size of the vector.
	 *
	 * \param n The new size. Note, it must a be lower or equal to the capacity.
	 */
	constexpr void resize(const bzd::SizeType n) noexcept { size_ = (n < capacity_) ? n : capacity_; }

  protected:
	const SizeType capacity_;
};
}  // namespace impl

namespace interface {
template <class T>
using Vector = impl::Vector<T, Span<T>>;
}

template <class T, SizeType N>
class Vector : public interface::Vector<T>
{
  public:
	template <class... Args>
	constexpr explicit Vector(Args &&... args) : interface::Vector<T>(N, data_, sizeof...(Args)), data_{bzd::forward<Args>(args)...}
	{
	}

  protected:
	T data_[N];
};
}  // namespace bzd
