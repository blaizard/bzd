#include "bzd.h"

int main()
{
	
	/*class Date
	{
	public:
		Date() = default;

		constexpr Date(bzd::UInt16Type y, bzd::UInt16Type m, bzd::UInt16Type d) : y_{y}, m_{m}, d_{d} {}

	private:
		bzd::UInt16Type y_;
		bzd::UInt16Type m_;
		bzd::UInt16Type d_;
	};*/

	bzd::format::toString(bzd::getOut(), CSTR("The answer is {}.\n"), 42);
	
	//Date date{2020, 8, 4};
	//bzd::format::toString(bzd::getOut(), CSTR("The answer is {}.\n"), date);

	return 0;
}
