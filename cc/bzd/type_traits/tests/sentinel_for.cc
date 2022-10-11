#include "cc/bzd/type_traits/sentinel_for.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/type_traits/sized_sentinel_for.hh"

TEST(TypeTraits, SentinelFor)
{
	{
		const auto result = bzd::concepts::sentinelFor<int*, int*>;
		EXPECT_TRUE(result);
	}
	{
		const auto result = bzd::concepts::sentinelFor<int*, float*>;
		EXPECT_FALSE(result);
	}
	{
		bzd::Array<int, 12u> array{};
		const auto result = bzd::concepts::sentinelFor<decltype(array.begin()), decltype(array.end())>;
		EXPECT_TRUE(result);
	}
}

TEST(TypeTraits, SizedSentinelFor)
{
	{
		const auto result = bzd::concepts::sizedSentinelFor<int*, int*>;
		EXPECT_TRUE(result);
	}
	{
		const auto result = bzd::concepts::sizedSentinelFor<int*, float*>;
		EXPECT_FALSE(result);
	}
	{
		bzd::Array<int, 12u> array{};
		const auto result = bzd::concepts::sizedSentinelFor<decltype(array.begin()), decltype(array.end())>;
		EXPECT_TRUE(result);
		EXPECT_EQ(array.end() - array.begin(), 12);
	}
	{
		bzd::Array<int, 12u> array{};
		const auto result = bzd::concepts::sizedSentinelFor<bzd::typeTraits::RangeIterator<decltype(array)>,
															bzd::typeTraits::RangeSentinel<decltype(array)>>;
		EXPECT_TRUE(result);
	}
}
