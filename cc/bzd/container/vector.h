#pragma once

#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"
#include "bzd/utility/forward.h"
#include "bzd/container/impl/span.h"
#include "bzd/container/storage/resizeable.h"
#include "bzd/container/storage/non_owning.h"

namespace bzd::impl {
template <class T, class Storage>
class Vector : public impl::Span<T, Storage>
{
protected:
	using Parent = impl::Span<T, Storage>;
	using StorageType = typename Parent::StorageType;

public:
	constexpr explicit Vector(const Storage& storage, const bzd::SizeType capacity) noexcept : Parent{Storage{storage}}, capacity_{capacity}
	{
	}

	/**
	 * \brief Adds a new element at the end of the vector, after its current last
	 * element.
	 * The content of val is copied (or moved) to the new element.
	 * \param value Value to be copied (or moved) to the new element.
	 */
	constexpr void pushBack(const T& value)
	{
		bzd::assert::isTrue(this->size() < capacity_, "Out of bound");
		++this->storage_.sizeMutable();
		this->at(this->size() - 1) = value;
	}

	/**
	 * Appends a new element to the end of the container.
	 * The element is constructed through using placement-new in-place at the location provided by the iterator.
	 * \param args... Arguments forwarded to the constructor.
	 */
	template <class... Args>
	constexpr void emplaceBack(Args&&... args)
	{
		bzd::assert::isTrue(this->size() < capacity_, "Out of bound");
		auto it = this->end();
		this->emplace(it, bzd::forward<Args>(args)...);
		++this->storage_.sizeMutable();
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
	constexpr void resize(const bzd::SizeType n) noexcept { this->storage_.sizeMutable() = (n < capacity_) ? n : capacity_; }

protected:
	const SizeType capacity_;
};
} // namespace bzd::impl

namespace bzd::interface {
template <class T>
using Vector = impl::Vector<T, impl::NonOwningStorage<T>>;
}

namespace bzd {
template <class T, SizeType N>
class Vector : public interface::Vector<T>
{
protected:
	using Parent = interface::Vector<T>;
	using StorageType = typename Parent::StorageType;

public:
	constexpr Vector() noexcept : Parent{StorageType{data_, 0}, N} {}

	template <class... Args>
	constexpr explicit Vector(Args&&... args) noexcept : Parent{StorageType{data_, sizeof...(Args)}, N}, data_{bzd::forward<Args>(args)...}
	{
	}

private:
	T data_[N]{};
};

template <class T, SizeType N>
class VectorConstexpr : public impl::Vector<T, impl::ResizeableStorage<T, N>>
{
protected:
	using Parent = impl::Vector<T, impl::ResizeableStorage<T, N>>;
	using StorageType = typename Parent::StorageType;

public:
	constexpr VectorConstexpr() noexcept : Parent{StorageType{}, N} {}

	template <class... Args>
	constexpr explicit VectorConstexpr(Args&&... args) noexcept : Parent{StorageType{bzd::forward<Args>(args)...}, N}
	{
	}
};
} // namespace bzd
