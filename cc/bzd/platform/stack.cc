#include "bzd/platform/stack.h"

#include "bzd/platform/types.h"

namespace bzd::platform::impl {
/**
 * Switch from one stack1 to stack2 and update stack1 pointer.
 *
 * \note It is important that both stack1 and stack2 are pointer of pointer
 * to support th ecase when it points to the same object.
 *
 * \param stack1 Pointing to the current stack, this is where the pointer will be updated.
 * \param stack2 The pointer to the stack to switch to.
 */
extern "C" void stackSwitch(bzd::PtrType* stack1, bzd::PtrType* stack2);
} // namespace bzd::platform::impl

void bzd::platform::interface::Stack::shift(Stack& stack) noexcept
{
	bzd::platform::impl::stackSwitch(&stack_, &stack.stack_);
}

void bzd::platform::interface::StackUser::taint(UInt8Type pattern) noexcept
{
	bzd::algorithm::fill(stackBase_, stackBase_ + size_, pattern);
}

bzd::SizeType bzd::platform::interface::StackUser::estimateMaxUsage(UInt8Type pattern) const noexcept
{
	if (direction_ == Direction::DOWNWARD)
	{
		bzd::SizeType i{0};
		for (; i < size_ && stackBase_[i] == pattern; ++i)
		{
		}
		return size_ - i;
	}

	bzd::SizeType i{size_ - 1};
	for (; i >= 0 && stackBase_[i] == pattern; --i)
	{
	}
	return i;
}