#include "example/format/format.hh"

class Date
{
public:
	Date() = default;

	constexpr Date(bzd::UInt16 y, bzd::UInt16 m, bzd::UInt16 d) : y_{y}, m_{m}, d_{d} {}

private:
	friend struct bzd::FormatterAsync<Date>;

	bzd::UInt16 y_;
	bzd::UInt16 m_;
	bzd::UInt16 d_;
};

namespace bzd {
template <>
struct FormatterAsync<Date>
{
	static bzd::Async<> toStream(bzd::OStream& os, const Date& d)
	{
		co_await !bzd::toStream(os, "{:}:{:}:{:}"_csv, int(d.y_), int(d.m_), int(d.d_));
		co_return {};
	}
};
} // namespace bzd

namespace example {

bzd::Async<> run(bzd::OStream& out)
{
	auto scope = co_await out.getLock();
	co_await !toStream(out, "The answer is {}.\n"_csv, 52);

	const Date date{2020, 8, 4};
	co_await !toStream(out, "This date {} is {:.2%} true!\n{}\n"_csv, date, 0.85, "Hello World!"_sv);

	bzd::String<128> str;
	bzd::toString(str.assigner(), "This date {}."_csv, 12);
	std::cout << str.data() << std::endl;

	co_return {};
}

} // namespace example
