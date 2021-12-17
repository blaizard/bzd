#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::test {

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

} // namespace bzd::test
