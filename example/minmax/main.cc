#include "cc/bzd.hh"

auto minmax(int i1, int i2, int i3, int i4) noexcept -> bzd::Tuple<int, int>
{
	auto min = bzd::min(i1, i2, i3, i4);
	auto max = bzd::max(i1, i2, i3, i4);

	return bzd::Tuple<int, int>(min, max);
}

int main()
{
	const auto result = minmax(4, -8, 6, 10);
	bzd::format::toStream(bzd::platform::out(), CSTR("min: {}, max: {}\n"), result.get<0>(), result.get<1>()).sync();

	return 0;
}
