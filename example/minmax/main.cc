#include "cc/bzd.hh"

auto minmax(int i1, int i2, int i3, int i4) noexcept -> bzd::Tuple<int, int>
{
	auto min = bzd::min(i1, i2, i3, i4);
	auto max = bzd::max(i1, i2, i3, i4);

	return bzd::Tuple<int, int>(bzd::inPlace, min, max);
}
/*
int main()
{
	const auto [min, max] = minmax(4, -8, 6, 10);
	bzd::format::toStream(bzd::platform::out(), "min: {}, max: {}\n"_csv, min, max).sync();

	return 0;
}
*/
