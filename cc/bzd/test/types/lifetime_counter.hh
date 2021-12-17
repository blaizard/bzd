#pragma once

#include "cc/bzd/platform/types.hh"

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
	constexpr LifetimeCounter() noexcept { ++constructor_; }
	constexpr LifetimeCounter(const Self& other) noexcept { *this = other; }
	constexpr Self& operator=(const Self& other) noexcept
	{
		constructor_ = other.constructor_;
		copy_ = other.copy_ + 1;
		move_ = other.move_;
		destructor_ = other.destructor_;
		return *this;
	}
	constexpr LifetimeCounter(Self&& other) noexcept { *this = bzd::move(other); }
	constexpr Self& operator=(Self&& other) noexcept
	{
		constructor_ = other.constructor_;
		copy_ = other.copy_;
		move_ = other.move_ + 1;
		destructor_ = other.destructor_;
		return *this;
	}

	~LifetimeCounter() noexcept { ++destructor_; }

public:
	static bzd::SizeType constructor_;
	static bzd::SizeType copy_;
	static bzd::SizeType move_;
	static bzd::SizeType destructor_;
};

template <class T>
bzd::SizeType LifetimeCounter<T>::constructor_{0};
template <class T>
bzd::SizeType LifetimeCounter<T>::copy_{0};
template <class T>
bzd::SizeType LifetimeCounter<T>::move_{0};
template <class T>
bzd::SizeType LifetimeCounter<T>::destructor_{0};

} // namespace bzd::test
