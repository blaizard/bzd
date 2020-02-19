#include "bzd/signal.h"
#include "cc_test/test.h"

TEST(Signal, Base)
{
	bzd::ConstBuffer buffer("HelloWorld");

	bzd::Signal<char, 8, 8> charSignal2;
	EXPECT_EQ(charSignal2.get(buffer), 'e');

	bzd::Signal<char, 40, 8> charSignal5;
	EXPECT_EQ(charSignal5.get(buffer), 'W');

	//bzd::Signal<char, 4, 8> charSignalShift;
	//EXPECT_EQ(charSignalShift.get(buffer), 0x00);
}
