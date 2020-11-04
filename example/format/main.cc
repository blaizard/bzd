#include "bzd.h"

class Date
{
public:
	Date() = default;

	constexpr Date(bzd::UInt16Type y, bzd::UInt16Type m, bzd::UInt16Type d) : y_{y}, m_{m}, d_{d} {}

	bzd::UInt16Type y_;
	bzd::UInt16Type m_;
	bzd::UInt16Type d_;
};

void toString(bzd::OChannel& os, const Date& d)
{
	bzd::format::toString(os, CSTR("{:.4}:{:.2}:{:.2}"), int(d.y_), int(d.m_), int(d.d_));
}

int main()
{
	bzd::format::toString(bzd::platform::getOut(), CSTR("The answer is {}.\n"), 42);

	const Date date{2020, 8, 4};
	bzd::format::toString(bzd::platform::getOut(), CSTR("This date {} is {:.2%} true!\n"), date, 0.85);

	return 0;
}
