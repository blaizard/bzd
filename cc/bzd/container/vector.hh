#pragma once

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
	constexpr explicit Vector(const Storage& storage, const bzd::SizeType capacity) noexcept : Parent{Storage{storage}}, capacity_{capacity}
	{
	}

	/// \brief Adds a new element at the end of the vector, after its current last
	/// element.
	/// The content of val is copied (or moved) to the new element.
	///
	/// \param value Value to be copied (or moved) to the new element.
	constexpr void pushBack(const T& value)
	{
		bzd::assert::isTrue(this->size() < capacity_, "Out of bound");
		++this->storage_.sizeMutable();
		this->at(this->size() - 1) = value;
	}

	/// Appends a new element to the end of the container.
	/// The element is constructed through using placement-new in-place at the location provided by the iterator.
	///
	/// \param args... Arguments forwarded to the constructor.
	template <class... Args>
	constexpr void emplaceBack(Args&&... args)
	{
		bzd::assert::isTrue(this->size() < capacity_, "Out of bound");
		auto it = this->end();
		this->emplace(it, bzd::forward<Args>(args)...);
		++this->storage_.sizeMutable();
	}

	/// \brief Returns the maximum number of elements the vector can hold.
	///
	/// \return Maximum number of element this vector can hold.
	constexpr SizeType capacity() const noexcept { return capacity_; }

	/// \brief Removes all elements.
	constexpr void clear() noexcept { resize(0); }

	/// \brief Change the size of the vector.
	///
	/// \param n The new size. Note, it must a be lower or equal to the capacity.
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
public: // Traits.
	using Self = Vector<T, N>;
	using Parent = interface::Vector<T>;
	using StorageType = typename Parent::StorageType;
	using ValueType = typename Parent::ValueType;

public: // Constructors/assignments.
	constexpr Vector() noexcept : Parent{StorageType{data_, 0}, N} {}
	constexpr Vector(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr Vector(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	template <class... Args>
	constexpr Vector(InPlace, Args&&... args) noexcept : Parent{StorageType{data_, sizeof...(Args)}, N}, data_{bzd::forward<Args>(args)...}
	{
	}

	template <class... Args>
	constexpr Vector(std::initializer_list<ValueType> list) : Parent{StorageType{data_, sizeof...(Args)}, N}, data_{}
	{
		for (const auto& value : list)
		{
			this->pushBack(value);
		}
	}

private:
	T data_[N]{};
};

template <class T, SizeType N>
class VectorConstexpr : public impl::Vector<T, impl::ResizeableStorage<T, N>>
{
public: // Traits.
	using Self = VectorConstexpr<T, N>;
	using Parent = impl::Vector<T, impl::ResizeableStorage<T, N>>;
	using StorageType = typename Parent::StorageType;

public: // Constructors/assignments.
	constexpr VectorConstexpr() noexcept : Parent{StorageType{}, N} {}
	constexpr VectorConstexpr(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr VectorConstexpr(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	template <class... Args>
	constexpr VectorConstexpr(InPlace, Args&&... args) noexcept : Parent{StorageType{bzd::forward<Args>(args)...}, N}
	{
	}
};
} // namespace bzd
