#include "bzd/container/named_type.h"

#include "cc_test/test.h"

TEST(NamedType, simple)
{
	using Kilo = bzd::NamedType<int, struct KiloTag, bzd::Arithmetic>;
	Kilo kg{12};
	EXPECT_EQ(kg.get(), 12);
	const Kilo constKg{13};
	EXPECT_EQ(constKg.get(), 13);

	// Incrementable
	++kg;
	EXPECT_EQ(kg.get(), 13);
	kg++;
	EXPECT_EQ(kg.get(), 14);

	// Decrementable
	--kg;
	EXPECT_EQ(kg.get(), 13);
	kg--;
	EXPECT_EQ(kg.get(), 12);

	// Addable
	{
		kg += constKg;
		EXPECT_EQ(kg.get(), 25);
		const auto result = kg + constKg;
		EXPECT_EQ(kg.get(), 25);
		EXPECT_EQ(result.get(), 38);
	}

	// Subtractable
	{
		kg -= constKg;
		EXPECT_EQ(kg.get(), 12);
		const auto result = kg - constKg;
		EXPECT_EQ(kg.get(), 12);
		EXPECT_EQ(result.get(), -1);
	}

	// Multiplicable
	{
		kg *= constKg;
		EXPECT_EQ(kg.get(), 156);
		const auto result = kg * constKg;
		EXPECT_EQ(kg.get(), 156);
		EXPECT_EQ(result.get(), 2028);
	}

	// Divisible
	{
		kg /= constKg;
		EXPECT_EQ(kg.get(), 12);
		const auto result = kg / constKg;
		EXPECT_EQ(kg.get(), 12);
		EXPECT_EQ(result.get(), 0);
	}

	// Modulable
	{
		const Kilo number{5};
		kg %= number;
		EXPECT_EQ(kg.get(), 2);
		const auto result = kg % number;
		EXPECT_EQ(kg.get(), 2);
		EXPECT_EQ(result.get(), 2);
	}
}
