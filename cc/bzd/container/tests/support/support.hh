#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::test {
class MoveOnly
{
protected:
	using Self = MoveOnly;

public:
	constexpr MoveOnly() noexcept = default;
	constexpr MoveOnly(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr MoveOnly(Self&& move) noexcept { *this = bzd::move(move); }
	constexpr Self& operator=(Self&& move) noexcept
	{
		movedCounter_ = move.movedCounter_ + 1;
		move.hasBeenMoved_ = true;
		return *this;
	}

public:
	[[nodiscard]] constexpr bzd::SizeType getMovedCounter() noexcept { return movedCounter_; }
	[[nodiscard]] constexpr bzd::BoolType hasBeenMoved() noexcept { return hasBeenMoved_; }

private:
	bzd::SizeType movedCounter_{0};
	bzd::BoolType hasBeenMoved_{false};
};

} // namespace bzd::test
