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

void toStringCustom(bzd::OStream& /*os*/, const Date& /*d*/)
{
	//bzd::format::toString(os, CSTR("{}:{:.2}:{:.2}"), int(d.y_), int(d.m_), int(d.d_));
}

int main()
{
	bzd::format::toString(bzd::getOut(), CSTR("The answer is {}.\n"), 42);
	
	//Date date{2020, 8, 4};
	//bzd::format::toString(bzd::getOut(), CSTR("The answer is {}.\n"), date);

	return 0;
}
