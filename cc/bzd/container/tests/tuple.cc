#include "cc/bzd/container/tuple.hh"

#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/move.hh"

TEST(ContainerTuple, Base)
{
	bzd::Tuple<int, bool, double, int> tuple;

	// Accessor by index as template

	tuple.get<0>() = 12;
	tuple.get<1>() = true;
	tuple.get<2>() = 2.4;
	tuple.get<3>() = -32;

	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_EQ(tuple.get<1>(), true);
	EXPECT_NEAR(tuple.get<2>(), 2.4, 0.0001);
	EXPECT_EQ(tuple.get<3>(), -32);
}

TEST(ContainerTuple, Constructor)
{
	bzd::Tuple<unsigned int, bool, double, int, const char*> tuple{bzd::inPlace, 12u, true, 5.32, -21, "Hello"};

	EXPECT_EQ(tuple.get<0>(), 12U);
	EXPECT_EQ(tuple.get<1>(), true);
	EXPECT_NEAR(tuple.get<2>(), 5.32, 0.0001);
	EXPECT_EQ(tuple.get<3>(), -21);
	EXPECT_STREQ(tuple.get<4>(), "Hello");
}

TEST(ContainerTuple, ConstructorPartial)
{
	bzd::Tuple<unsigned int, bool, double, int> tuple{bzd::inPlace, 12u, true};

	EXPECT_EQ(tuple.get<0>(), 12U);
	EXPECT_EQ(tuple.get<1>(), true);
}

TEST(ContainerTuple, Const)
{
	bzd::Tuple<const int, const bool, const double, const char*> tuple{bzd::inPlace, 12, false, 8.7, "Hello World"};
	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_EQ(tuple.get<1>(), false);
}

TEST(ContainerTuple, Constexpr)
{
	constexpr const bzd::Tuple<int, const char*> tuple{bzd::inPlace, 12, "Hello"};
	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_STREQ(tuple.get<1>(), "Hello");
}

TEST(ContainerTuple, ConstexprStringView)
{
	constexpr const bzd::Tuple<int, bzd::StringView> tuple2{bzd::inPlace, 12, "Hello"};
	EXPECT_EQ(tuple2.get<0>(), 12);
	EXPECT_STREQ(tuple2.get<1>().data(), "Hello");
}

TEST(ContainerTuple, NonDefaultConstructor)
{
	class NonDefaultConstructor
	{
	public:
		constexpr explicit NonDefaultConstructor(int v) : value{v} {}
		int value;
	};

	constexpr const bzd::Tuple<NonDefaultConstructor, NonDefaultConstructor> tuple{bzd::inPlace, 12, -2};
	EXPECT_EQ(tuple.get<0>().value, 12);
	EXPECT_EQ(tuple.get<1>().value, -2);

	auto tupleCopy{tuple};
	EXPECT_EQ(tupleCopy.get<0>().value, 12);

	auto tupleMove{bzd::move(tuple)};
	EXPECT_EQ(tupleMove.get<0>().value, 12);
}

TEST(ContainerTuple, Copy)
{
	class NonDefaultConstructor
	{
	public:
		constexpr explicit NonDefaultConstructor(int v) : value{v} {}
		int value;
	};

	const bzd::Tuple<int, char, NonDefaultConstructor> tuple{bzd::inPlace, 12, 'a', 3};
	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_EQ(tuple.get<1>(), 'a');
	EXPECT_EQ(tuple.get<2>().value, 3);

	const auto tupleCopy{tuple};
	EXPECT_EQ(tupleCopy.get<0>(), 12);
	EXPECT_EQ(tupleCopy.get<1>(), 'a');
	EXPECT_EQ(tupleCopy.get<2>().value, 3);

	const auto tupleCopy2 = tuple;
	EXPECT_EQ(tupleCopy2.get<0>(), 12);
	EXPECT_EQ(tupleCopy2.get<1>(), 'a');
	EXPECT_EQ(tupleCopy2.get<2>().value, 3);
}

TEST(ContainerTuple, Move)
{
	class NonDefaultConstructor
	{
	public:
		constexpr explicit NonDefaultConstructor(int v) : value{v} {}
		int value;
	};

	bzd::Tuple<int, char, NonDefaultConstructor> tuple{bzd::inPlace, 12, 'a', 3};
	EXPECT_EQ(tuple.get<0>(), 12);
	EXPECT_EQ(tuple.get<1>(), 'a');
	EXPECT_EQ(tuple.get<2>().value, 3);

	auto tupleMove{bzd::move(tuple)};
	EXPECT_EQ(tupleMove.get<0>(), 12);
	EXPECT_EQ(tupleMove.get<1>(), 'a');
	EXPECT_EQ(tupleMove.get<2>().value, 3);

	const auto tupleMove2 = bzd::move(tupleMove);
	EXPECT_EQ(tupleMove2.get<0>(), 12);
	EXPECT_EQ(tupleMove2.get<1>(), 'a');
	EXPECT_EQ(tupleMove2.get<2>().value, 3);
}

TEST(ContainerTuple, Iterator)
{
	bzd::Tuple<unsigned int, bool, double, int, const char*> tuple{bzd::inPlace, 12u, true, 5.32, -21, "Hello"};

	auto it = tuple.begin();

	unsigned int value{0};
	(*it).match([&value](unsigned int& v) { value = v; }, [](auto&) {});
	EXPECT_EQ(value, 12u);
}
