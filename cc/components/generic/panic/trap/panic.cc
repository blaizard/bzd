#include "cc/bzd/platform/panic.hh"

void bzd::platform::panic(const bzd::FunctionRef<void(bzd::OStream&)>)
{
	while (1)
	{
	};
}
