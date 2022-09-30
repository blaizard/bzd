#include "cc/bzd/container/range/subrange.hh"

#include "cc/bzd/container/vector.hh"
#include "cc/bzd/test/test.hh"

TEST(SubRange, Simple)
{
	bzd::Vector<bzd::UInt32, 12> vector;

	bzd::range::SubRange emptyVectorView{vector.begin(), vector.end()};

	static_assert(bzd::concepts::begin<decltype(emptyVectorView)>, "SubRange must validate the `begin` concept.");
	static_assert(bzd::concepts::end<decltype(emptyVectorView)>, "SubRange must validate the `end` concept.");
	static_assert(bzd::concepts::size<decltype(emptyVectorView)>, "SubRange must validate the `size` concept.");
	static_assert(bzd::concepts::range<decltype(emptyVectorView)>, "SubRange must be considered as a range.");

	EXPECT_TRUE(emptyVectorView.empty());
	EXPECT_EQ(emptyVectorView.size(), 0);

	vector.pushBack(1u);
	vector.pushBack(2u);
	vector.pushBack(3u);
	bzd::range::SubRange nonEmptyVectorView{vector.begin(), vector.end()};
	EXPECT_FALSE(nonEmptyVectorView.empty());
	EXPECT_EQ(nonEmptyVectorView.size(), 3);
}
