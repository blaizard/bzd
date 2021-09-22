#include "cc/bzd/core/logger/minimal.h"

#include "cc/bzd/core/logger/tests/support/backend.h"
#include "cc_test/test.h"

TEST(Minimal, Empty)
{
	bzd::minimal::log::info("hello");
}

TEST(Minimal, Backend)
{
	bzd::test::Logger<1024> backend;
	bzd::backend::Logger::setDefault(backend);

	// Simple message
	bzd::minimal::log::error("hello");
	EXPECT_TRUE(backend.match("[e] [minimal.cc:17] hello"_sv.asBytes()));
	bzd::minimal::log::warning("hello");
	EXPECT_TRUE(backend.match("[w] [minimal.cc:19] hello"_sv.asBytes()));
	bzd::minimal::log::info("hello");
	EXPECT_TRUE(backend.match("[i] [minimal.cc:21] hello"_sv.asBytes()));

	// Default level is INFO, this will be hidden
	bzd::minimal::log::debug("hello");
	EXPECT_FALSE(backend.match("[d] [minimal.cc:25] hello"_sv.asBytes()));
}
