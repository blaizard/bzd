#include "cc/bzd/utility/align_down.hh"

#include "cc/bzd/test/test.hh"

TEST(AlignDown, Base)
{
	{
		const auto result = bzd::alignDown<2>(13);
		EXPECT_EQ(result, 12);
	}
	{
		const auto result = bzd::alignDown<2>(12);
		EXPECT_EQ(result, 12);
	}
	{
		const auto result = bzd::alignDown<2>(0);
		EXPECT_EQ(result, 0);
	}
}

TEST(AlignDown, TypeIntegrals)
{
	{
		const auto result = bzd::alignDown<2>(static_cast<bzd::UInt8Type>(13));
		EXPECT_EQ(result, 12);
	}
	{
		const auto result = bzd::alignDown<8>(static_cast<bzd::UInt16Type>(13));
		EXPECT_EQ(result, 8);
	}
	{
		const auto result = bzd::alignDown<8>(static_cast<bzd::UInt64Type>(13));
		EXPECT_EQ(result, 8);
	}
}

TEST(AlignDown, TypePointers)
{
	{
		const auto result = bzd::alignDown<8>(reinterpret_cast<void*>(13));
		EXPECT_EQ(result, reinterpret_cast<void*>(8));
	}
	{
		const auto result = bzd::alignDown<16>(reinterpret_cast<int*>(0x327632));
		EXPECT_EQ(result, reinterpret_cast<int*>(0x327630));
	}
	{
		const auto result = bzd::alignDown<64>(reinterpret_cast<bzd::UInt64Type*>(0x799493));
		EXPECT_EQ(result, reinterpret_cast<bzd::UInt64Type*>(0x799480));
	}
}
