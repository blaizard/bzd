#include "cc/bzd.h"

class Date
{
public:
	Date() = default;

	constexpr Date(bzd::UInt16Type y, bzd::UInt16Type m, bzd::UInt16Type d) : y_{y}, m_{m}, d_{d} {}

	bzd::UInt16Type y_;
	bzd::UInt16Type m_;
	bzd::UInt16Type d_;
};

void toStream(bzd::OChannel& os, const Date& d)
{
	bzd::format::toStream(os, CSTR("{:.4}:{:.2}:{:.2}"), int(d.y_), int(d.m_), int(d.d_));
}

void toStream(bzd::OChannel& os, const bzd::StringView& view)
{
	os.write(view.asBytes());
}

int main()
{
	bzd::format::toStream(bzd::platform::getOut(), CSTR("The answer is {}.\n"), 42);

	const Date date{2020, 8, 4};
	bzd::format::toStream(bzd::platform::getOut(), CSTR("This date {} is {:.2%} true!\n{}\n"), date, 0.85, "Hello World!"_sv);

	bzd::String<128> str;
	bzd::format::toString(str, "This date {}.", 12);
	std::cout << str.data() << std::endl;

	return 0;
}
