#include "bzd/container/named_type.h"

#include "cc_test/test.h"

TEST(NamedType, arithmetic)
{
	using Kilo = bzd::NamedType<int, struct KiloTag, bzd::Arithmetic>;
	Kilo kg{12};
	EXPECT_EQ(kg.get(), 12);
	const Kilo constKg{13};
	EXPECT_EQ(constKg.get(), 13);

	// Empty
	{
		Kilo empty{};
		EXPECT_EQ(empty.get(), 0);
	}

	// Copy
	{
		const Kilo construct{kg};
		EXPECT_EQ(construct.get(), 12);
		Kilo assign;
		assign = kg;
		EXPECT_EQ(assign.get(), 12);
	}

	// Move
	{
		Kilo toMove{32};
		const Kilo construct{bzd::move(toMove)};
		EXPECT_EQ(construct.get(), 32);
		Kilo toMoveAssign{34};
		Kilo assign;
		assign = bzd::move(toMoveAssign);
		EXPECT_EQ(assign.get(), 34);
	}

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
		EXPECT_EQ((kg * 2).get(), 312);
	}

	// Divisible
	{
		kg /= constKg;
		EXPECT_EQ(kg.get(), 12);
		const auto result = kg / constKg;
		EXPECT_EQ(kg.get(), 12);
		EXPECT_EQ(result.get(), 0);
		EXPECT_EQ((kg / 2).get(), 6);
	}

	// Modulable
	{
		const Kilo number{5};
		kg %= number;
		EXPECT_EQ(kg.get(), 2);
		const auto result = kg % number;
		EXPECT_EQ(kg.get(), 2);
		EXPECT_EQ(result.get(), 2);
		EXPECT_EQ((kg % 2).get(), 0);
	}

	// Comparable
	{
		const Kilo equal{2};
		const Kilo lower{-3};
		const Kilo greater{42};

		EXPECT_FALSE(kg == lower);
		EXPECT_FALSE(kg == greater);
		EXPECT_TRUE(kg == equal);

		EXPECT_TRUE(kg != lower);
		EXPECT_TRUE(kg != greater);
		EXPECT_FALSE(kg != equal);

		EXPECT_TRUE(kg > lower);
		EXPECT_FALSE(kg > greater);
		EXPECT_FALSE(kg > equal);

		EXPECT_TRUE(kg >= lower);
		EXPECT_FALSE(kg >= greater);
		EXPECT_TRUE(kg >= equal);

		EXPECT_FALSE(kg < lower);
		EXPECT_TRUE(kg < greater);
		EXPECT_FALSE(kg < equal);

		EXPECT_FALSE(kg <= lower);
		EXPECT_TRUE(kg <= greater);
		EXPECT_TRUE(kg <= equal);
	}

	// BitOperation
	{
		const Kilo number{4};
		kg |= number;
		kg &= number;
		kg = ~kg;
		kg ^= number;
		kg <<= number;
		kg >>= number;
		EXPECT_EQ(kg.get(), -1);
		kg <<= 2;
		kg >>= 2;
		EXPECT_EQ(kg.get(), -1);
	}
}

TEST(NamedType, multipleOf)
{
	using Kilogram = bzd::NamedType<int, struct KiloTag, bzd::Arithmetic>;
	using Pound = bzd::MultipleOf<Kilogram, bzd::Ratio<56699, 125000>, bzd::Arithmetic>;

	// Exact conversion
	{
		const Kilogram kg{Pound{125000}};
		EXPECT_EQ(kg.get(), 56699);
		const Pound pound{kg};
		EXPECT_EQ(pound.get(), 125000);
	}

	// Round
	{
		const Kilogram kg{Pound{13}};
		EXPECT_EQ(kg.get(), 6);
		const Pound pound{Kilogram{3}};
		EXPECT_EQ(pound.get(), 7);
	}

	// Skills inheritance
	{
		const Kilogram kg{3};
		const Pound pound{12};
		{
			auto result = kg + pound;
			EXPECT_EQ(result.get(), 3 + 5);
		}
		{
			auto result = pound + kg;
			EXPECT_EQ(result.get(), 7 + 12);
		}
		Pound copy{pound};
		EXPECT_EQ(copy.get(), 12);
		copy += pound;
		EXPECT_EQ(copy.get(), 24);
	}
}
