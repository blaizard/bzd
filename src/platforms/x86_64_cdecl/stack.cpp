#include "include/interface/stack.h"

const async::interface::Stack::Direction async::interface::Stack::direction_ = async::interface::Stack::Direction::DOWNWARD;

// See https://web.archive.org/web/20160801075146/http://www.x86-64.org/documentation/abi.pdf

void async::interface::Stack::reset(const fct_ptr_type fct, void* context)
{
	auto pStack = last<void*, 16>();
	//pStack[0] is for the return address
	// check why we cannot use -1 or 0
	pStack[-2] = context;
	pStack[-3] = reinterpret_cast<void*>(fct);
	stack_ = reinterpret_cast<byte_type*>(pStack - 10);
}
