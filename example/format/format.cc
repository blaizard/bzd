#include "example/format/format.hh"

class Date
{
public:
	Date() = default;

	constexpr Date(bzd::UInt16Type y, bzd::UInt16Type m, bzd::UInt16Type d) : y_{y}, m_{m}, d_{d} {}

	bzd::UInt16Type y_;
	bzd::UInt16Type m_;
	bzd::UInt16Type d_;
};

bzd::Async<void> toStream(bzd::OStream& os, const Date& d)
{
	co_await toStream(os, CSTR("{:.4}:{:.2}:{:.2}"), int(d.y_), int(d.m_), int(d.d_));
}

namespace Example {

bzd::Async<bool> run()
{
	auto scope = co_await bzd::platform::out().getLock();
	co_await toStream(bzd::platform::out(), CSTR("The answer is {}.\n"), 42);

	const Date date{2020, 8, 4};
	co_await toStream(bzd::platform::out(), CSTR("This date {} is {:.2%} true!\n{}\n"), date, 0.85, "Hello World!"_sv);

	bzd::String<128> str;
	toString(str, CSTR("This date {}."), 12);
	std::cout << str.data() << std::endl;

	co_return true;
}

} // namespace Example
