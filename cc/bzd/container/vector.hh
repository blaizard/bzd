#pragma once

#include "cc/bzd/algorithm/copy.hh"
#include "cc/bzd/algorithm/move.hh"
#include "cc/bzd/container/impl/span_resizeable.hh"
#include "cc/bzd/container/storage/non_owning.hh"
#include "cc/bzd/container/storage/resizeable.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_trivially_default_constructible.hh"
#include "cc/bzd/type_traits/is_trivially_destructible.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/address_of.hh"
#include "cc/bzd/utility/construct_at.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/in_place.hh"

#include <initializer_list>

namespace bzd::impl {
template <class T, class Storage>
class Vector : public impl::SpanResizeable<T, Storage>
{
public: // Traits.
	using Self = Vector<T, Storage>;
	using Parent = impl::SpanResizeable<T, Storage>;
	using StorageType = typename Parent::StorageType;
	using ValueType = T;

public:
	constexpr explicit Vector(const Storage& storage, const bzd::Size capacity) noexcept : Parent{Storage{storage}, capacity} {}

public: // API.
	/// Appends the given element value to the end of the container.
	template <class... Args>
	constexpr Size pushBack(Args&&... args) noexcept
	{
		return Parent::append(bzd::forward<Args>(args)...);
	}

	/// Removes the last element in the vector, effectively reducing the container size by one.
	constexpr void popBack() noexcept
	{
		const auto size = Parent::size();
		if (size)
		{
			Parent::resize(size - 1);
		}
	}
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
	constexpr Vector() noexcept : Parent{StorageType{rawData(), 0}, capacity} {}
	constexpr Vector(const Self& other) noexcept : Vector{} { *this = other; }
	constexpr Self& operator=(const Self& other) noexcept
	{
		if (this != &other)
		{
			this->resize(0);
			for (const auto& element : other)
			{
				this->pushBack(element);
			}
		}
		return *this;
	}
	constexpr Vector(Self&& other) noexcept : Vector{} { *this = bzd::move(other); }
	constexpr Self& operator=(Self&& other) noexcept
	{
		if (this != &other)
		{
			this->resize(0);
			for (auto&& element : other)
			{
				this->pushBack(bzd::move(element));
			}
		}
		return *this;
	}

	template <class... Args>
	constexpr Vector(InPlace, Args&&... args) noexcept : Vector{}
	{
		(this->pushBack(args), ...);
	}

	template <class... Args>
	constexpr Vector(std::initializer_list<ValueType> list) : Vector{}
	{
		for (const auto& value : list)
		{
			this->pushBack(value);
		}
	}

	template <Size otherCapacity>
	requires(otherCapacity < capacity)
	constexpr Vector(const Vector<T, otherCapacity>& other) noexcept : Vector{}
	{
		this->resize(0);
		for (const auto& element : other)
		{
			this->pushBack(element);
		}
	}

	template <Size otherCapacity>
	requires(otherCapacity < capacity)
	constexpr Vector(Vector<T, otherCapacity>&& other) noexcept : Vector{}
	{
		this->resize(0);
		for (auto&& element : other)
		{
			this->pushBack(bzd::move(element));
		}
	}

private:
	static constexpr Bool isSufficientlyTrivial = typeTraits::isTriviallyDefaultConstructible<T> && typeTraits::isTriviallyDestructible<T>;
	constexpr T* rawData() noexcept
	{
		if constexpr (isSufficientlyTrivial)
		{
			return static_cast<T*>(data_);
		}
		else
		{
			return reinterpret_cast<T*>(data_);
		}
	}
	// Idea borrowed from: https://www.youtube.com/watch?v=I8QJLGI0GOE
	using DataStorageType = typeTraits::Conditional<isSufficientlyTrivial, T[capacity], bzd::Byte[sizeof(T) * capacity]>;
	alignas(T) DataStorageType data_{};
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
