#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd::test {

/// Type that contains lifetime counters, that could be used for monitoring
/// proper handling of the object through copy and move constructors and assignments
/// or within the object scope.
template <class T>
class LifetimeCounter
{
protected:
	using Self = LifetimeCounter;

public:
	constexpr LifetimeCounter() noexcept { ++constructor; }
	constexpr LifetimeCounter(const Self& other) noexcept { *this = other; }
	constexpr Self& operator=(const Self& other) noexcept
	{
		constructor = other.constructor;
		copy = other.copy + 1;
		move = other.move;
		destructor = other.destructor;
		return *this;
	}
	constexpr LifetimeCounter(Self&& other) noexcept { *this = bzd::move(other); }
	constexpr Self& operator=(Self&& other) noexcept
	{
		constructor = other.constructor;
		copy = other.copy;
		move = other.move + 1;
		destructor = other.destructor;
		return *this;
	}

	~LifetimeCounter() noexcept { ++destructor; }

public:
	static bzd::Size constructor;
	static bzd::Size copy;
	static bzd::Size move;
	static bzd::Size destructor;
};

template <class T>
bzd::Size LifetimeCounter<T>::constructor{0};
template <class T>
bzd::Size LifetimeCounter<T>::copy{0};
template <class T>
bzd::Size LifetimeCounter<T>::move{0};
template <class T>
bzd::Size LifetimeCounter<T>::destructor{0};

} // namespace bzd::test
