#include "cc/bzd/core/logger/minimal.h"

#include "cc/bzd/container/string_view.h"
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
	EXPECT_TRUE(backend.match("[e] [minimal.cc:18] hello"_sv.asBytes()));
	bzd::minimal::log::warning("hello");
	EXPECT_TRUE(backend.match("[w] [minimal.cc:20] hello"_sv.asBytes()));
	bzd::minimal::log::info("hello");
	EXPECT_TRUE(backend.match("[i] [minimal.cc:22] hello"_sv.asBytes()));

	// Default level is INFO, this will be hidden
	bzd::minimal::log::debug("hello");
	EXPECT_FALSE(backend.match("[d] [minimal.cc:26] hello"_sv.asBytes()));
}
