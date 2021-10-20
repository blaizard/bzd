#include "cc/bzd/container/optional2.hh"

#include "cc/bzd/test/types.hh"
#include "cc_test/test.hh"

TEST(ContainerOptional2, simpleData)
{
	bzd::Optional2<int> v(42);

	EXPECT_TRUE(v);
	EXPECT_EQ(v.value(), 42);
	EXPECT_EQ(v.valueOr(10), 42);

	v.valueMutable() = 13;
	EXPECT_EQ(v.value(), 13);

	auto vCopy{v};
	EXPECT_TRUE(vCopy);
	EXPECT_EQ(vCopy.value(), 13);

	auto vMove{bzd::move(v)};
	EXPECT_TRUE(vMove);
	EXPECT_EQ(vMove.value(), 13);

	vCopy = vMove;
	EXPECT_TRUE(vCopy);
	EXPECT_EQ(vCopy.value(), 13);

	vMove = bzd::move(vCopy);
	EXPECT_TRUE(vMove);
	EXPECT_EQ(vMove.value(), 13);

	v.emplace(45);
	EXPECT_TRUE(v);
	EXPECT_EQ(v.value(), 45);

	v.reset();
	EXPECT_FALSE(v);
}

TEST(ContainerOptional2, copy)
{
	// Copy value
	bzd::test::CopyOnly value{};
	bzd::Optional2<bzd::test::CopyOnly> v{value};
	EXPECT_TRUE(v);
	EXPECT_EQ(v->getCopiedCounter(), 1);

	// Copy constructor
	bzd::Optional2<bzd::test::CopyOnly> vCopied{v};
	EXPECT_TRUE(vCopied);
	EXPECT_EQ(vCopied->getCopiedCounter(), 2);

	// Copy assignment
	bzd::Optional2<bzd::test::CopyOnly> vAssingnment;
	EXPECT_FALSE(vAssingnment);
	vAssingnment = vCopied;
	EXPECT_TRUE(vAssingnment);
	EXPECT_EQ(vAssingnment->getCopiedCounter(), 3);
}

TEST(ContainerOptional2, move)
{
	// Move value
	bzd::Optional2<bzd::test::MoveOnly> v{bzd::test::MoveOnly()};
	EXPECT_TRUE(v);
	EXPECT_FALSE(v->hasBeenMoved());
	EXPECT_EQ(v->getMovedCounter(), 1);

	// Move constructor
	bzd::Optional2<bzd::test::MoveOnly> vMoved{bzd::move(v)};
	EXPECT_TRUE(vMoved);
	EXPECT_FALSE(vMoved->hasBeenMoved());
	EXPECT_EQ(vMoved->getMovedCounter(), 2);

	// Move assignment
	bzd::Optional2<bzd::test::MoveOnly> vAssingnment;
	EXPECT_FALSE(vAssingnment);
	vAssingnment = bzd::move(vMoved);
	EXPECT_TRUE(vAssingnment);
	EXPECT_FALSE(vAssingnment->hasBeenMoved());
	EXPECT_EQ(vAssingnment->getMovedCounter(), 3);
}

TEST(ContainerOptional2, simpleNoData)
{
	bzd::Optional2<int> v;

	EXPECT_FALSE(v);
	EXPECT_EQ(v.valueOr(10), 10);
}

TEST(ContainerOptional2, constexprType)
{
	constexpr bzd::Optional2<int> v;

	EXPECT_FALSE(v);
	EXPECT_EQ(v.valueOr(10), 10);

	constexpr bzd::Optional2<int> u{23};

	EXPECT_TRUE(u);
	EXPECT_EQ(u.valueOr(10), 23);
}

TEST(ContainerOptional2, complexData)
{
	struct Value
	{
		int a;
	};
	bzd::Optional2<Value> v{Value{45}};

	EXPECT_TRUE(v);
	EXPECT_EQ(v.valueOr({12}).a, 45);
	EXPECT_EQ(v->a, 45);

	auto vCopy{v};
	EXPECT_TRUE(vCopy);
	EXPECT_EQ(vCopy->a, 45);

	auto vMove{bzd::move(vCopy)};
	EXPECT_TRUE(vMove);
	EXPECT_EQ(vMove->a, 45);

	v.emplace(Value{23});
	EXPECT_TRUE(v);
	EXPECT_EQ(v->a, 23);

	v.reset();
	EXPECT_FALSE(v);
}

/*
TEST(ContainerOptional2, reference)
{
	int a = 42;
	bzd::Optional<int&> v(a);

	EXPECT_TRUE(v);
	EXPECT_EQ(v.value(), 42);
	a = -85;
	EXPECT_EQ(v.value(), -85);

	int b = 12;
	v.emplace(b);
	EXPECT_TRUE(v);
	EXPECT_EQ(v.value(), 12);
	b = -25;
	EXPECT_EQ(v.value(), -25);

	auto vCopy{v};
	EXPECT_TRUE(vCopy);
	EXPECT_EQ(vCopy.value(), -25);
	b = 34;
	EXPECT_EQ(vCopy.value(), 34);

	auto vMove{bzd::move(vCopy)};
	EXPECT_TRUE(vMove);
	EXPECT_EQ(vMove.value(), 34);
	b = 7;
	EXPECT_EQ(vMove.value(), 7);
}
*/

TEST(ContainerOptional2, pointer)
{
	int a = 42;
	bzd::Optional2<int*> v(&a);

	EXPECT_TRUE(v);
	EXPECT_EQ(*(v.value()), 42);
	a = -85;
	EXPECT_EQ(*(v.value()), -85);

	auto vCopy{v};
	EXPECT_TRUE(vCopy);
	EXPECT_EQ(*(vCopy.value()), -85);
	a = 34;
	EXPECT_EQ(*(vCopy.value()), 34);

	auto vMove{bzd::move(vCopy)};
	EXPECT_TRUE(vMove);
	EXPECT_EQ(*(vMove.value()), 34);
	a = 7;
	EXPECT_EQ(*(vMove.value()), 7);
}