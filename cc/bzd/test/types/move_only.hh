#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd::test {

/// Type that implements only a move constructor and move assignment operator.
/// In other word, the copy constructor and copy assignment operator is deleted.
class MoveOnly
{
protected:
	using Self = MoveOnly;

public:
	/// Construct a move oonly object.
	///
	/// \param destroyedBeforeMoved A pointer to a boolean that will be set to true in case the object was destroyed before being moved.
	constexpr MoveOnly(bzd::Bool* const destroyedBeforeMoved = nullptr) noexcept : destroyedBeforeMoved_{destroyedBeforeMoved} {}
	constexpr MoveOnly(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;
	constexpr MoveOnly(Self&& move) noexcept { *this = bzd::move(move); }
	constexpr Self& operator=(Self&& move) noexcept
	{
		movedCounter_ = move.movedCounter_ + 1;
		destroyedBeforeMoved_ = move.destroyedBeforeMoved_;
		move.hasBeenMoved_ = 1;
		return *this;
	}
	constexpr ~MoveOnly() noexcept
	{
		if (!hasBeenMoved_ && destroyedBeforeMoved_)
		{
			*destroyedBeforeMoved_ = true;
		}
	}

public:
	[[nodiscard]] constexpr bzd::Size getMovedCounter() const noexcept { return movedCounter_; }
	[[nodiscard]] constexpr bzd::Bool hasBeenMoved() const noexcept { return hasBeenMoved_; }

private:
	bzd::Size movedCounter_{0};
	bzd::Bool hasBeenMoved_{false};
	bzd::Bool* destroyedBeforeMoved_{nullptr};
};

} // namespace bzd::test
