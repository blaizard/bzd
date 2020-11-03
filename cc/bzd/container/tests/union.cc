#include "bzd/meta/union.h"

#include "bzd/utility/move.h"
#include "cc_test/test.h"

#include <string>

TEST(MetaUnion, Constructor)
{
	bzd::meta::Union<int, bool, float> test;

	test.get<int>() = 21;
	EXPECT_EQ(test.get<int>(), 21);
	test.get<bool>() = true;
	EXPECT_TRUE(test.get<bool>());
	test.get<float>() = 3.435;
	EXPECT_NEAR(test.get<float>(), 3.435, 0.0001);

	// Constructor
	bzd::meta::Union<int, bool, float> testInt(static_cast<int>(42));
	EXPECT_EQ(testInt.get<int>(), 42);

	const bzd::meta::Union<int, bool, float> constTest(static_cast<bool>(true));
	EXPECT_TRUE(constTest.get<bool>());

	bzd::meta::Union<int, double> a{static_cast<double>(3.1415)};
	EXPECT_NEAR(a.get<double>(), 3.1415, 0.0001);
}

TEST(MetaUnionConstexpr, Constructor)
{
	constexpr bzd::meta::UnionConstexpr<int, bool, float> constexprTest(static_cast<float>(32.5));
	EXPECT_NEAR(constexprTest.get<float>(), 32.5, 0.001);

	// Constexpr Copy Constructor
	constexpr auto constexprTest2(constexprTest);
	constexpr auto constexprTest3 = constexprTest2;
	EXPECT_NEAR(constexprTest3.get<float>(), 32.5, 0.001);

	// Complex types
	bzd::meta::Union<bool, std::string> testComplex(std::string("Hello"));
	EXPECT_STREQ(testComplex.get<std::string>().c_str(), "Hello");
}
