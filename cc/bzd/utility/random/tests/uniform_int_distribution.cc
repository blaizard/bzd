#include "cc/bzd/utility/random/uniform_int_distribution.hh"

#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/random/linear_congruential_engine.hh"

TEST(UniformIntDistribution, Base)
{
	bzd::LinearCongruentialEngine<> generator_{};
	bzd::UniformIntDistribution<bzd::UInt32, 0, 10> distribution{};

	EXPECT_EQ(distribution(generator_), 5U);
	EXPECT_EQ(distribution(generator_), 6U);
	EXPECT_EQ(distribution(generator_), 5U);
	EXPECT_EQ(distribution(generator_), 4U);
}
