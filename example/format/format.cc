#include "example/format/format.hh"

class Date
{
public:
	Date() = default;

	constexpr Date(bzd::UInt16Type y, bzd::UInt16Type m, bzd::UInt16Type d) : y_{y}, m_{m}, d_{d} {}

private:
	friend bzd::Async<> toStream(bzd::OStream& os, const Date& d);

	bzd::UInt16Type y_;
	bzd::UInt16Type m_;
	bzd::UInt16Type d_;
};

bzd::Async<> toStream(bzd::OStream& os, const Date& d)
{
	co_await !toStream(os, "{:.4}:{:.2}:{:.2}"_csv, int(d.y_), int(d.m_), int(d.d_));
	co_return {};
}

namespace Example {

bzd::Async<> run()
{
	auto scope = co_await bzd::platform::out().getLock();
	co_await !toStream(bzd::platform::out(), "The answer is {}.\n"_csv, 52);

	const Date date{2020, 8, 4};
	co_await !toStream(bzd::platform::out(), "This date {} is {:.2%} true!\n{}\n"_csv, date, 0.85, "Hello World!"_sv);

	bzd::String<128> str;
	toString(str, "This date {}."_csv, 12);
	std::cout << str.data() << std::endl;

	co_return {};
}

} // namespace Example
