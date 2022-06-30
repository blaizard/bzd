#include "cc/bzd/utility/ownership.hh"

#include "cc/bzd/test/test.hh"

struct OwnershipTest : public bzd::Ownership<OwnershipTest>
{
	OwnershipTest(const bzd::Int32 v) : bzd::Ownership<OwnershipTest>{}, value{v} {}
	bzd::Int32 value;
};

TEST(Ownership, Base)
{
	OwnershipTest owner{12};
	EXPECT_EQ(owner.value, 12);
	EXPECT_EQ(owner.getBorrowedCounter(), 0U);

	{
		auto borrowed{owner.borrow()};
		EXPECT_EQ(owner.getBorrowedCounter(), 1U);
		EXPECT_EQ(borrowed->value, 12);
	}
	EXPECT_EQ(owner.getBorrowedCounter(), 0U);
}

TEST(Ownership, Const)
{
	const OwnershipTest owner{12};
	EXPECT_EQ(owner.value, 12);

	auto borrowed{owner.borrow()};
	EXPECT_EQ(borrowed->value, 12);
}
