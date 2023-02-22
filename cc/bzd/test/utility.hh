#include "cc/bzd/core/async.hh"

#include <iomanip>
#include <iostream>

namespace bzd::test {

template <class T>
void dump(const T& object)
{
	std::cout << "sizeof() = " << sizeof(object) << std::endl;
	for (std::size_t i = 0; i < sizeof(object); ++i)
	{
		std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex
				  << (static_cast<unsigned int>(reinterpret_cast<const char*>(&object)[i]) & 0xff) << " ";
	}
	std::cout << std::dec << std::endl;
}

bzd::Async<> delay(const bzd::Size count) noexcept
{
	for (bzd::Size i = 0; i < count; ++i)
	{
		co_await bzd::async::yield();
	}
	co_return {};
}

bzd::Async<> timeout(const bzd::Size count) noexcept
{
	co_await !delay(count);
	co_return bzd::error::Timeout("Operation timed out after {} ticks"_csv, count);
}

} // namespace bzd::test
