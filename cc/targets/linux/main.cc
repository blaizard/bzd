#include "cc/bzd/platform/panic.hh"
#include "cc/components/std/stream/out/out.hh"

void bzd::platform::panic(const bzd::FunctionRef<void(bzd::OStream&)> callback)
{
	::bzd::platform::std::Out out{};
	callback(out);
	throw 42;
}

int main()
{
	extern bool runExecutor() noexcept;
	if (runExecutor())
	{
		return 0;
	}
	return 1;
}
