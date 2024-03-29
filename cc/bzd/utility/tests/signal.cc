#include "cc/bzd/utility/signal.hh"

#include "cc/bzd/test/test.hh"

TEST(Signal, Base)
{
	char data[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x00};

	// 0x48 0x65 0x6c 0x6c 0x6f 0x57 0x6f 0x72 0x6c 0x64
	bzd::Span<char> buffer{data};

	bzd::Signal<char, 8, 8> charSignal2;
	EXPECT_EQ(charSignal2.get(buffer.asBytes()), 'e');

	bzd::Signal<char, 40, 8> charSignal5;
	EXPECT_EQ(charSignal5.get(buffer.asBytes()), 'W');

	charSignal5.set(buffer.asBytesMutable(), 'N');
	EXPECT_EQ(charSignal5.get(buffer.asBytes()), 'N');

	bzd::Signal<char, 4, 8> charSignalShift;
	EXPECT_EQ(charSignalShift.get(buffer.asBytes()), 0x54);

	charSignalShift.set(buffer.asBytesMutable(), 0x42);
	EXPECT_EQ(charSignalShift.get(buffer.asBytes()), 0x42);
}
