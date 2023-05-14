#include "cc/bzd/container/range/stream.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(Stream, Simple)
{
	bzd::Array<bzd::Int32, 12u> container{bzd::inPlace, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
	bzd::range::Stream stream{container};

	auto it1 = stream.begin();
	EXPECT_EQ(*it1, 0);
	++it1;
	EXPECT_EQ(*it1, 1);
	++it1;

	auto it2 = stream.begin();
	EXPECT_EQ(*it2, 2);
	++it2;

	EXPECT_EQ(*it1, 3);
	++it1;

	EXPECT_EQ(*it2, 4);
	++it2;
}

TEST(Stream, Nested)
{
	bzd::Array<bzd::Int32, 12u> container{bzd::inPlace, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

	bzd::range::Stream stream1{container};
	auto it1 = stream1.begin();
	EXPECT_EQ(*it1, 0);
	++it1;
	EXPECT_EQ(*it1, 1);
	++it1;

	bzd::range::Stream stream2{it1, stream1.end()};
	auto it2 = stream2.begin();
	EXPECT_EQ(*it2, 2);
	++it2;

	EXPECT_EQ(*it1, 3);
	++it1;

	EXPECT_EQ(*it2, 4);
	++it2;
}
