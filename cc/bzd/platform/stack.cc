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
extern "C" void stackSwitch(bzd::UInt8Type** stack1, bzd::UInt8Type** stack2);
} // namespace bzd::impl

void bzd::platform::interface::Stack::contextSwitch(Stack& stack) noexcept
{
	bzd::platform::impl::stackSwitch(&stack_, &stack.stack_);
}
