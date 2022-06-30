#pragma once

#include "cc/bzd/core/assert.hh"

namespace bzd {
template <class T>
class Ownership;

///
template <class T>
class Borrowed
{
public:
	constexpr Borrowed(T& resource) noexcept : resource_{&resource} { resource_->incrementCounter(); }
	constexpr Borrowed& operator=(const Borrowed&) noexcept = delete;
	constexpr Borrowed(const Borrowed& other) noexcept : Borrowed{static_cast<T&>(*other.resource_)} {}

	constexpr Borrowed& operator=(Borrowed&&) noexcept = delete;
	constexpr Borrowed(Borrowed&& other) noexcept : resource_{bzd::move(other.resource_)} { other.resource_ = nullptr; }

public:
	/// Accesses the owner.
	///
	/// \return Returns a pointer to the owner.
	[[nodiscard]] constexpr const T* operator->() const noexcept { return resource_; }

	/// Accesses the owner.
	///
	/// \return Returns a pointer to the owner.
	[[nodiscard]] constexpr T* operator->() noexcept { return resource_; }

	constexpr ~Borrowed() noexcept
	{
		if (resource_)
		{
			resource_->decrementCounter();
		}
	}

private:
	T* resource_;
};

/// Class providing ownership to a ressource.
///
/// It can borrow its resources to others and must ensure that all resources
/// have been returned before being destroyed.
/// It can also transfer its ownership iff its resource is not currently borrowed.
template <class T>
class Ownership
{
public:
	Ownership() = default;
	constexpr Ownership(const Ownership&) noexcept = delete;
	constexpr Ownership& operator=(const Ownership&) noexcept = delete;
	constexpr Ownership(Ownership&&) noexcept = default;
	constexpr Ownership& operator=(Ownership&&) noexcept = default;

public:
	/// Borrow this object, take a reference to it but do not give away its ownership.
	/// \{
	[[nodiscard]] constexpr Borrowed<T> borrow() noexcept
	{
		return static_cast<T&>(*this);
	}
	[[nodiscard]] constexpr Borrowed<const T> borrow() const noexcept
	{
		return static_cast<const T&>(*this);
	}
	/// \}

	/// Get the number of times this resource is currently borrowed.
	[[nodiscard]] constexpr bzd::UInt32Type getBorrowedCounter() const noexcept
	{
		return borrowedCounter_.load(MemoryOrder::relaxed);
	}

	/// Ensure all resources have been returned before destroying the object.
	constexpr ~Ownership() noexcept
	{
		const auto value{getBorrowedCounter()};
		bzd::assert::isTrue(value == 0, "{} dangling borrowed resource(s)."_csv, value);
	}

private:
	friend class Borrowed<T>;
	friend class Borrowed<const T>;

	constexpr void incrementCounter() const noexcept { borrowedCounter_.fetchAdd(1, MemoryOrder::relaxed); }
	constexpr void decrementCounter() const noexcept { borrowedCounter_.fetchSub(1, MemoryOrder::acquireRelease); }

private:
	/// Number of times this resource is borrowed.
	mutable bzd::Atomic<bzd::UInt32Type> borrowedCounter_{0};
};

} // namespace bzd
