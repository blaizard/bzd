#pragma once

#include "cc/bzd/core/assert.hh"

namespace bzd {
template <class T>
class Owner;

///
template <class T>
class Borrowed
{
public:
	constexpr Borrowed(T& owner) noexcept : owner_{&owner} { owner_->incrementCounter(); }
	constexpr Borrowed& operator=(const Borrowed&) noexcept = delete;
	constexpr Borrowed(const Borrowed& other) noexcept : Borrowed{*other.owner_} {}

	constexpr Borrowed& operator=(Borrowed&&) noexcept = delete;
	constexpr Borrowed(Borrowed&& other) noexcept : owner_{bzd::move(other.owner_)} { other.owner_ = nullptr; }

public:
	constexpr T& get() noexcept { return static_cast<T&>(*owner_); }
	constexpr const T& get() const noexcept { return static_cast<const T&>(*owner_); }

	constexpr ~Borrowed() noexcept
	{
		if (owner_)
		{
			owner_->decrementCounter();
		}
	}

private:
	Owner<T>* owner_;
};

/// Class owning a ressource.
///
/// It can borrow its resources to others and must ensure that all resources
/// have been returned before being destroyed.
/// It can also transfer its ownership iff its resource is not currently borrowed.
template <class T>
class Owner
{
public:
	constexpr Owner() noexcept = default;
	constexpr Owner(const Owner&) noexcept = delete;
	constexpr Owner& operator=(const Owner&) noexcept = delete;
	constexpr Owner(Owner&&) noexcept = default;
	constexpr Owner& operator=(Owner&&) noexcept = default;

public:
	/// Borrow this object, take a reference to it but do not give away its ownership.
	/// \{
	[[nodiscard]] constexpr Borrowed<T> borrow() noexcept
	{
		incrementCounter();
		return Borrowed{*this};
	}
	[[nodiscard]] constexpr Borrowed<const T> borrow() const noexcept
	{
		incrementCounter();
		return Borrowed{*this};
	}
	/// \}

	/// Ensure all resources have been returned before destroying the object.
	constexpr ~Owner() noexcept
	{
		const auto value{borrowedCounter_.load(MemoryOrder::relaxed)};
		bzd::assert::isTrue(value == 0, "{} dangling borrowed resource(s)."_csv, value);
	}

private:
	friend class Borrowed<T>;
	friend class Borrowed<const T>;

	constexpr void incrementCounter() const noexcept { borrowedCounter_.fetchAdd(1, MemoryOrder::relaxed); }
	constexpr void decrementCounter() const noexcept { borrowedCounter_.fetchSub(1, MemoryOrder::acquireRelease); }

private:
	/// Number of times this resource is borrowed.
	mutable bzd::Atomic<bzd::Int32Type> borrowedCounter_{0};
};

} // namespace bzd
