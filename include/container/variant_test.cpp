#include "gtest/gtest.h"
#include "include/container/variant.h"

TEST(ContainerVariant, Constructor)
{
	bzd::Variant<int, bool, double> variant;
	bzd::Variant<int, bool, double> variantInt(static_cast<int>(45));
	bzd::Variant<int, bool, double> variantBool(static_cast<bool>(true));
	bzd::Variant<int, bool, double> variantDouble(static_cast<double>(5.4));
}
