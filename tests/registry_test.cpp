#include "gtest/gtest.h"
#include "bzd/core/registry.h"

TEST(Registry, Base)
{
	bzd::Registry<int, 3> reg_;

	//Registry<double>::get("hello")

    //EXPECT_EQ(span.size(), 5);
}

// Registry are used to store logger, communication channels
// Data transfer are used by loggers
// static constexpr const registryNbLog
