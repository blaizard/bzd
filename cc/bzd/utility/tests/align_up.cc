#include "bzd/utility/align_up.h"

#include "cc_test/test.h"

TEST(AlignUp, Base)
{
	{
		const auto result = bzd::alignUp<2>(13);
		EXPECT_EQ(result, 14);
	}
	{
		const auto result = bzd::alignUp<2>(12);
		EXPECT_EQ(result, 12);
	}
	{
		const auto result = bzd::alignUp<2>(0);
		EXPECT_EQ(result, 0);
	}
}

TEST(AlignUp, TypeIntegrals)
{
	{
		const auto result = bzd::alignUp<2>(static_cast<bzd::UInt8Type>(13));
		EXPECT_EQ(result, 14);
	}
	{
		const auto result = bzd::alignUp<8>(static_cast<bzd::UInt16Type>(13));
		EXPECT_EQ(result, 16);
	}
	{
		const auto result = bzd::alignUp<8>(static_cast<bzd::UInt64Type>(13));
		EXPECT_EQ(result, 16);
	}
}

TEST(AlignUp, TypePointers)
{
	{
		const auto result = bzd::alignUp<8>(reinterpret_cast<void*>(13));
		EXPECT_EQ(result, reinterpret_cast<void*>(16));
	}
	{
		const auto result = bzd::alignUp<16>(reinterpret_cast<int*>(0x327632));
		EXPECT_EQ(result, reinterpret_cast<int*>(0x327640));
	}
	{
		const auto result = bzd::alignUp<64>(reinterpret_cast<bzd::UInt64Type*>(0x799493));
		EXPECT_EQ(result, reinterpret_cast<bzd::UInt64Type*>(0x7994c0));
	}
}