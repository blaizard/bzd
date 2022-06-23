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
	Borrowed(T& owner) : owner_{owner} {}

	T& get() noexcept { return static_cast<T&>(owner_); }
	const T& get() const noexcept { return static_cast<const T&>(owner_); }

	~Borrowed() { --owner_.borrowedCounter_; }

private:
	Owner<T>& owner_;
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
	[[nodiscard]] constexpr Borrowed<T> borrow() noexcept { return Borrowed{*this}; }

	/// Ensure all resources have been returned before destroying the object.
	constexpr ~Owner() noexcept { bzd::assert::isTrue(borrowedCounter_ == 0, "{} dangling borrowed resource(s)."_csv, borrowedCounter_); }

private:
	friend class Borrowed<T>;
	/// Number of times this resource is borrowed.
	bzd::Int32Type borrowedCounter_{0};
};

} // namespace bzd
