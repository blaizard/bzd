#pragma once

#include "cc/bzd/algorithm/fill.hh"
#include "cc/bzd/container/array.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd {

/**
 * Describes the direction to which the stack grows.
 */
enum class StackDirection : UInt8Type
{
	/**
	 * The stack grows from lower address to higher
	 */
	UPWARD = 0,
	/**
	 * The stack grows from higher address to lower
	 */
	DOWNWARD = 1
};

template <bzd::SizeType stackSize, StackDirection direction = StackDirection::DOWNWARD>
class Stack
{
public: // Type.
	using DataType = bzd::UInt8Type;

public:
	constexpr Stack() noexcept = default;

	constexpr void taint(UInt8Type pattern = 0xaa) noexcept { bzd::algorithm::fill(stack_.begin(), stack_.end(), pattern); }

	constexpr DataType* data() noexcept { return stack_.data(); }

	constexpr SizeType size() const noexcept { return stack_.size(); }

	bzd::SizeType estimateMaxUsage(UInt8Type pattern = 0xaa) const noexcept
	{
		if constexpr (direction == StackDirection::DOWNWARD)
		{
			bzd::SizeType i{0};
			for (; i < stack_.size() && stack_[i] == pattern; ++i)
			{
			}
			return stack_.size() - i;
		}
		else
		{
			bzd::SizeType i{stack_.size() - 1};
			for (; i > 0 && stack_[i] == pattern; --i)
			{
			}
			return i;
		}
	}

private:
	bzd::Array<DataType, stackSize> stack_{};
};

} // namespace bzd
