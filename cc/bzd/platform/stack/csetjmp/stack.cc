#include "cc/bzd/platform/stack.h"

#include <csetjmp>

const bzd::platform::interface::Stack::Direction bzd::platform::interface::Stack::direction_ =
	bzd::platform::interface::Stack::Direction::DOWNWARD;

void bzd::platform::interface::StackUser::reset(const FctPtrType fct) noexcept
{
	volatile void* stackPtr;
	auto pStack = last<bzd::UInt8Type, 16>();

	// Save and set stack pointer to stackBase_
	__asm__ __volatile__("mov %%rsp, %0" : "=r"(stackPtr));
	__asm__ __volatile__("mov %0, %%rsp" : : "r"(pStack));

	__asm__ __volatile__("" ::: "memory"); // prevent compiler reorderings
	{
		std::jmp_buf buffer;
		if (setjmp(buffer) != 0)
		{
			fct();
		}
	}
	__asm__ __volatile__("" ::: "memory"); // prevent compiler reorderings

	// Set back stack pointer to current stack
	__asm__ __volatile__("mov %0, %%rsp" : : "r"(stackPtr));

	stack_ = static_cast<bzd::PtrType>(pStack - sizeof(std::jmp_buf));
}

extern "C" void stackSwitch(bzd::PtrType* stack1, bzd::PtrType* stack2)
{
	std::jmp_buf buffer;
	*stack1 = &buffer;

	if (setjmp(buffer) != 0)
	{
		return;
	}
	longjmp(*static_cast<std::jmp_buf*>(*stack2), 1);
}
