#include "include/interface/stack.h"

const async::interface::Stack::Direction async::interface::Stack::direction_ = async::interface::Stack::Direction::GROW_BACKWARD;

// See https://web.archive.org/web/20160801075146/http://www.x86-64.org/documentation/abi.pdf

void async::interface::Stack::reset(const fct_ptr_type fct, void* context)
{
	auto pStack = last<void*, 16>();
	pStack[-2] = context;
	pStack[-3] = reinterpret_cast<void*>(fct); // pStack[0] is for the return address
	stack_ = reinterpret_cast<byte_type*>(pStack - 10);
}

/*
#include <stdarg.h>

int maxof(int n_args, ...)
{
    va_list ap;
    va_start(ap, n_args);
    int max = va_arg(ap, int);
    for(int i = 2; i <= n_args; i++) {
        int a = va_arg(ap, int);
        if(a > max) max = a;
    }
    va_end(ap);
    return max;
}
*/