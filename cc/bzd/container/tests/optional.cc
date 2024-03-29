#include "cc/bzd/container/optional.hh"

#include "cc/bzd/container/result.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/copy_only.hh"
#include "cc/bzd/test/types/move_only.hh"

TEST(ContainerOptional, simpleData)
{
	bzd::Optional<int> v(42);

	EXPECT_TRUE(v);
	EXPECT_EQ(v.value(), 42);
	EXPECT_EQ(v.valueOrAsCopy(10), 42);

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

	vMove.emplace(45);
	EXPECT_TRUE(vMove);
	EXPECT_EQ(vMove.value(), 45);

	vMove.reset();
	EXPECT_FALSE(vMove);
}

TEST(ContainerOptional, Comparison)
{
	bzd::Optional<int> v1{};
	bzd::Optional<int> v2{};
	EXPECT_EQ(v1, v2);
	bzd::Optional<int> v3{45};
	EXPECT_NE(v1, v3);
	bzd::Optional<int> v4{4};
	EXPECT_NE(v3, v4);
	bzd::Optional<int> v5{45};
	EXPECT_EQ(v3, v5);
}

TEST(ContainerOptional, copy)
{
	// Copy value
	bzd::test::CopyOnly value{};
	bzd::Optional<bzd::test::CopyOnly> v{value};
	EXPECT_TRUE(v);
	EXPECT_EQ(v->getCopiedCounter(), 1U);

	// Copy constructor
	bzd::Optional<bzd::test::CopyOnly> vCopied{v};
	EXPECT_TRUE(vCopied);
	EXPECT_EQ(vCopied->getCopiedCounter(), 2U);

	// Copy assignment
	bzd::Optional<bzd::test::CopyOnly> vAssingnment;
	EXPECT_FALSE(vAssingnment);
	vAssingnment = vCopied;
	EXPECT_TRUE(vAssingnment);
	EXPECT_EQ(vAssingnment->getCopiedCounter(), 3U);
}

TEST(ContainerOptional, move)
{
	// Move value
	bzd::Optional<bzd::test::MoveOnly> v{bzd::test::MoveOnly()};
	EXPECT_TRUE(v);
	EXPECT_FALSE(v->hasBeenMoved());
	EXPECT_EQ(v->getMovedCounter(), 1U);

	// Move constructor
	bzd::Optional<bzd::test::MoveOnly> vMoved{bzd::move(v)};
	EXPECT_TRUE(vMoved);
	EXPECT_FALSE(vMoved->hasBeenMoved());
	EXPECT_EQ(vMoved->getMovedCounter(), 2U);

	// Move assignment
	bzd::Optional<bzd::test::MoveOnly> vAssingnment;
	EXPECT_FALSE(vAssingnment);
	vAssingnment = bzd::move(vMoved);
	EXPECT_TRUE(vAssingnment);
	EXPECT_FALSE(vAssingnment->hasBeenMoved());
	EXPECT_EQ(vAssingnment->getMovedCounter(), 3U);
}

TEST(ContainerOptional, simpleNoData)
{
	bzd::Optional<int> v;

	EXPECT_FALSE(v);
	EXPECT_EQ(v.valueOrAsCopy(10), 10);
}

TEST_CONSTEXPR_BEGIN(ContainerOptional, Constexpr)
{
	constexpr bzd::Optional<int> v;

	EXPECT_FALSE(v);
	EXPECT_EQ(v.valueOrAsCopy(10), 10);

	constexpr bzd::Optional<int> u{23};

	EXPECT_TRUE(u);
	EXPECT_EQ(u.valueOrAsCopy(10), 23);

	constexpr auto vCopy{u};
	EXPECT_TRUE(vCopy);
	EXPECT_EQ(vCopy.value(), 23);

	constexpr auto vMove{bzd::move(vCopy)};
	EXPECT_TRUE(vMove);
	EXPECT_EQ(vMove.value(), 23);
}
TEST_CONSTEXPR_END(ContainerOptional, Constexpr)

TEST(ContainerOptional, complexData)
{
	struct Value
	{
		int a;
	};
	bzd::Optional<Value> v{Value{45}};

	EXPECT_TRUE(v);
	EXPECT_EQ(v.valueOrAsCopy({12}).a, 45);
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

TEST(ContainerOptional, multiArgsData)
{
	struct Value
	{
		Value(int a, int b) : a_{a}, b_{b} {}
		int a_;
		int b_;
	};

	bzd::Optional<Value> v{23, 45};
	EXPECT_TRUE(v);
	EXPECT_EQ(v.value().a_, 23);
	EXPECT_EQ(v.value().b_, 45);
}

TEST(ContainerOptional, reference)
{
	int a = 42;
	bzd::Optional<int&> v(a);

	EXPECT_TRUE(v);
	EXPECT_EQ(v.value(), 42);
	a = -85; // NOLINT(clang-analyzer-deadcode.DeadStores)
	EXPECT_EQ(v.value(), -85);

	int b = 12;
	v.emplace(b);
	EXPECT_TRUE(v);
	EXPECT_EQ(v.value(), 12);
	b = -25; // NOLINT(clang-analyzer-deadcode.DeadStores)
	EXPECT_EQ(v.value(), -25);

	auto vCopy{v};
	EXPECT_TRUE(vCopy);
	EXPECT_EQ(vCopy.value(), -25);
	b = 34; // NOLINT(clang-analyzer-deadcode.DeadStores)
	EXPECT_EQ(vCopy.value(), 34);

	auto vMove{bzd::move(vCopy)};
	EXPECT_TRUE(vMove);
	EXPECT_EQ(vMove.value(), 34);
	b = 7; // NOLINT(clang-analyzer-deadcode.DeadStores)
	EXPECT_EQ(vMove.value(), 7);
}

TEST(ContainerOptional, pointer)
{
	int a = 42;
	bzd::Optional<int*> v(&a);

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

TEST(ContainerOptional, result)
{
	{
		bzd::Optional<bzd::Result<int, bool>> v(45);

		EXPECT_TRUE(v);
		EXPECT_EQ(v.valueOrAsCopy({12}).value(), 45);

		auto vCopy{v};
		EXPECT_TRUE(vCopy);
		EXPECT_EQ(vCopy.value().value(), 45);

		auto vMove{bzd::move(vCopy)};
		EXPECT_TRUE(vMove);
		EXPECT_EQ(vMove.value().value(), 45);
	}

	{
		bzd::Optional<bzd::Result<int, bool>> v{};

		EXPECT_FALSE(v);
		EXPECT_EQ(v.valueOrAsCopy({12}).value(), 12);

		v.emplace(43);
		EXPECT_TRUE(v);
		EXPECT_TRUE(v.value());
		EXPECT_EQ(v.valueOrAsCopy({12}).value(), 43);

		v.emplace(bzd::error::make(false));
		EXPECT_TRUE(v);
		EXPECT_FALSE(v.value());
		EXPECT_FALSE((v.value()).error());
	}
}
