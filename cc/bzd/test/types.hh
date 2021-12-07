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

/// Type that implements a deleted default constructor.
class NoDefaultConstructor
{
protected:
	using Self = NoDefaultConstructor;

public:
	constexpr NoDefaultConstructor() noexcept = delete;
	constexpr NoDefaultConstructor(int) noexcept {}
	constexpr NoDefaultConstructor(const Self& copy) noexcept = default;
	constexpr Self& operator=(const Self& copy) noexcept = default;
	constexpr NoDefaultConstructor(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;
};

/// Type that implements only a copy constructor and copy assignment operator.
/// In other word, the move constructor and move assignment operator is deleted.
class CopyOnly
{
protected:
	using Self = CopyOnly;

public:
	constexpr CopyOnly() noexcept = default;
	constexpr CopyOnly(const Self& copy) noexcept { *this = copy; }
	constexpr Self& operator=(const Self& copy) noexcept
	{
		copiedCounter_ = copy.copiedCounter_ + 1;
		return *this;
	}
	constexpr CopyOnly(Self&&) noexcept = delete;
	constexpr Self& operator=(Self&&) noexcept = delete;

public:
	[[nodiscard]] constexpr bzd::SizeType getCopiedCounter() const noexcept { return copiedCounter_; }

private:
	bzd::SizeType copiedCounter_{0};
};

/// Type that implements only a move constructor and move assignment operator.
/// In other word, the copy constructor and copy assignment operator is deleted.
class MoveOnly
{
protected:
	using Self = MoveOnly;

public:
	constexpr MoveOnly() noexcept = default;
	constexpr MoveOnly(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr MoveOnly(Self&& move) noexcept { *this = bzd::move(move); }
	Self& operator=(Self&& move) noexcept
	{
		movedCounter_ = move.movedCounter_ + 1;
		move.hasBeenMoved_ = 1;
		return *this;
	}

public:
	[[nodiscard]] constexpr bzd::SizeType getMovedCounter() const noexcept { return movedCounter_; }
	[[nodiscard]] constexpr bzd::BoolType hasBeenMoved() const noexcept { return hasBeenMoved_; }

private:
	bzd::SizeType movedCounter_{0};
	bzd::BoolType hasBeenMoved_{false};
};

} // namespace bzd::test
