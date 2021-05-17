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
	bzd::format::toString(os, "{:.4}:{:.2}:{:.2}"_sv, int(d.y_), int(d.m_), int(d.d_));
}

void toString(bzd::OChannel& os, const bzd::StringView& view)
{
	os.write(view.asBytes());
}

int main()
{
	bzd::format::toString(bzd::platform::getOut(), "The answer is {}.\n"_sv, 42);

	const Date date{2020, 8, 4};
	bzd::format::toString(bzd::platform::getOut(), "This date {} is {:.2%} true!\n{}\n"_sv, date, 0.85, "Hello World!"_sv);

	return 0;
}
