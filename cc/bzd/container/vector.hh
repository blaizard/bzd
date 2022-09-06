#pragma once

#include "cc/bzd/algorithm/copy.hh"
#include "cc/bzd/algorithm/move.hh"
#include "cc/bzd/container/impl/span.hh"
#include "cc/bzd/container/storage/non_owning.hh"
#include "cc/bzd/container/storage/resizeable.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/in_place.hh"

#include <initializer_list>

namespace bzd::impl {
template <class T, class Storage>
class Vector : public impl::Span<T, Storage>
{
public: // Traits.
	using Self = Vector<T, Storage>;
	using Parent = impl::Span<T, Storage>;
	using StorageType = typename Parent::StorageType;
	using ValueType = T;

public:
	constexpr explicit Vector(const Storage& storage, const bzd::Size capacity) noexcept : Parent{Storage{storage}}, capacity_{capacity} {}

	/// \brief Adds a new element at the end of the vector, after its current last
	/// element.
	/// The content of val is copied (or moved) to the new element.
	///
	/// \param value Value to be copied (or moved) to the new element.
	template <class U>
	constexpr void pushBack(U&& value) noexcept
	{
		bzd::assert::isTrue(this->size() < capacity_, "Out of bound");
		++this->storage_.sizeMutable();
		this->at(this->size() - 1) = bzd::forward<U>(value);
	}

	/// Appends a new element to the end of the container.
	/// The element is constructed through using placement-new in-place at the location provided by the iterator.
	///
	/// \param args... Arguments forwarded to the constructor.
	template <class... Args>
	constexpr void emplaceBack(Args&&... args) noexcept
	{
		bzd::assert::isTrue(this->size() < capacity_, "Out of bound");
		auto it = this->end();
		this->emplace(it, bzd::forward<Args>(args)...);
		++this->storage_.sizeMutable();
	}

	/// TO be changed
	template <class Iterator>
	requires concepts::forwardIterator<Iterator>
	constexpr void insertBack(Iterator first, Iterator last) noexcept
	{
		while (!(first == last))
		{
			pushBack(*first);
			++first;
		}
	}

	/// \copydoc insertBack
	template <class Range>
	requires concepts::forwardRange<Range>
	constexpr void insertBack(Range&& range) { insertBack(bzd::begin(range), bzd::end(range)); }

	/// \brief Returns the maximum number of elements the vector can hold.
	///
	/// \return Maximum number of element this vector can hold.
	constexpr Size capacity() const noexcept { return capacity_; }

	/// \brief Removes all elements.
	constexpr void clear() noexcept { resize(0); }

	/// \brief Change the size of the vector.
	///
	/// \param n The new size. Note, it must a be lower or equal to the capacity.
	constexpr void resize(const bzd::Size n) noexcept { this->storage_.sizeMutable() = (n < capacity_) ? n : capacity_; }

protected:
	const Size capacity_;
};
} // namespace bzd::impl

namespace bzd::interface {
template <class T>
using Vector = impl::Vector<T, impl::NonOwningStorage<T>>;
}

namespace bzd {
template <class T, Size capacity>
class Vector : public interface::Vector<T>
{
public: // Traits.
	using Self = Vector<T, capacity>;
	using Parent = interface::Vector<T>;
	using StorageType = typename Parent::StorageType;
	using ValueType = typename Parent::ValueType;

public: // Constructors/assignments.
	constexpr Vector() noexcept : Parent{StorageType{data_, 0}, capacity} {}
	constexpr Vector(const Self& other) noexcept : Vector{} { *this = other; }
	constexpr Self& operator=(const Self& other) noexcept
	{
		this->resize(other.size());
		algorithm::copy(other, *this);
		return *this;
	}
	constexpr Vector(Self&& other) noexcept : Vector{} { *this = bzd::move(other); }
	constexpr Self& operator=(Self&& other) noexcept
	{
		this->resize(other.size());
		algorithm::move(bzd::move(other), *this);
		return *this;
	}

	template <class... Args>
	constexpr Vector(InPlace, Args&&... args) noexcept :
		Parent{StorageType{data_, sizeof...(Args)}, capacity}, data_{bzd::forward<Args>(args)...}
	{
	}

	template <Size otherCapacity>
	requires(otherCapacity < capacity) constexpr Vector(const Vector<T, otherCapacity>& other) noexcept : Vector{}
	{
		this->resize(other.size());
		algorithm::copy(other, *this);
	}

	template <Size otherCapacity>
	requires(otherCapacity < capacity) constexpr Vector(Vector<T, otherCapacity>&& other) noexcept : Vector{}
	{
		this->resize(other.size());
		algorithm::move(bzd::move(other), *this);
	}

private:
	T data_[capacity]{};
};

template <class T, Size capacity>
class VectorConstexpr : public impl::Vector<T, impl::ResizeableStorage<T, capacity>>
{
public: // Traits.
	using Self = VectorConstexpr<T, capacity>;
	using Parent = impl::Vector<T, impl::ResizeableStorage<T, capacity>>;
	using StorageType = typename Parent::StorageType;

public: // Constructors/assignments.
	constexpr VectorConstexpr() noexcept : Parent{StorageType{}, capacity} {}
	constexpr VectorConstexpr(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr VectorConstexpr(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	template <class... Args>
	constexpr VectorConstexpr(InPlace, Args&&... args) noexcept : Parent{StorageType{bzd::forward<Args>(args)...}, capacity}
	{
	}
};
} // namespace bzd
