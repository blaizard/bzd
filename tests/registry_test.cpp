#include "gtest/gtest.h"
#include "bzd/core/registry.h"

TEST(Registry, Base)
{
	bzd::Registry<double>::Declare<3> reg_;
	bzd::Registry<double> hello("hello", 12);

    EXPECT_EQ(bzd::Registry<double>::get("hello"), 12);
}

// Registry are used to store logger, communication channels
// Data transfer are used by loggers
// static constexpr const registryNbLog
