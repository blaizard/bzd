#include "cc/bzd/container/range/views/stream.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/test/test.hh"

TEST(Stream, Simple)
{
	bzd::Array<bzd::Int32, 12u> container{bzd::inPlace, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
	auto stream = bzd::range::Stream{bzd::inPlace, container};

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

	auto stream1 = bzd::range::Stream(bzd::inPlace, container);
	auto it1 = stream1.begin();
	EXPECT_EQ(*it1, 0);
	++it1;
	EXPECT_EQ(*it1, 1);
	++it1;

	bzd::range::Stream stream2{bzd::move(stream1)};
	auto it2 = stream2.begin();
	EXPECT_EQ(*it2, 2);
	++it2;

	EXPECT_EQ(*it2, 3);
	++it2;

	auto stream3 = bzd::range::Stream(bzd::move(stream2));
	auto it3 = stream3.begin();
	EXPECT_EQ(*it3, 4);
	++it3;
}

TEST(Stream, TypeTraits)
{
	bzd::Array<bzd::Int32, 12u> container;
	auto stream = bzd::range::Stream(bzd::inPlace, container);

	constexpr auto isInputOrOutputReference1 =
		bzd::concepts::sameTemplate<bzd::typeTraits::RangeIterator<decltype(stream)>, bzd::iterator::InputOrOutputReference>;
	EXPECT_TRUE(isInputOrOutputReference1);

	auto it = stream.begin();
	constexpr auto isInputOrOutputReference2 = bzd::concepts::sameTemplate<decltype(it), bzd::iterator::InputOrOutputReference>;
	EXPECT_TRUE(isInputOrOutputReference2);
}
