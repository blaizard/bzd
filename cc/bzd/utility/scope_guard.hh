#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd {
template <class T>
class ScopeGuard
{
public: // Traits
	using Self = ScopeGuard<T>;

public: // Constructors
	explicit constexpr ScopeGuard(const T& cleanup) noexcept : cleanup_{static_cast<T>(cleanup)} {}

	// Copy constructor/assignment
	constexpr ScopeGuard(const Self& scope) noexcept = delete;
	constexpr Self& operator=(const Self& scope) noexcept = delete;

	// Move constructor/assignment
	constexpr ScopeGuard(Self&& scope) noexcept : cleanup_{bzd::move(scope.cleanup_)} { scope.cleanup_.reset(); }
	constexpr Self& operator=(Self&& scope) noexcept
	{
		cleanup_ = bzd::move(scope.cleanup_);
		scope.cleanup_.reset();
	}

	/// Release the scope before the destructor.
	constexpr void release() noexcept
	{
		if (cleanup_)
		{
			cleanup_.value()();
			cleanup_.reset();
		}
	}

	/// Destructor
	constexpr ~ScopeGuard() noexcept
	{
		if (cleanup_)
		{
			cleanup_.value()();
		}
	}

private:
	bzd::Optional<T> cleanup_;
};
} // namespace bzd
