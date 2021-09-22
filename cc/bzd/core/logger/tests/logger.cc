#include "cc/bzd/core/logger.h"

#include "cc/bzd/core/logger/tests/support/backend.h"
#include "cc_test/test.h"

TEST(Logger, Empty)
{
	bzd::log::info(CSTR("hello")).sync();
}

TEST(Logger, Backend)
{
	bzd::test::Logger<1024> backend;
	bzd::backend::Logger::setDefault(backend);
	bzd::Logger::getDefault().setMinimumLevel(bzd::log::Level::DEBUG);

	// Simple message
	bzd::log::error(CSTR("hello")).sync();
	EXPECT_TRUE(backend.match("[e] [logger.cc:18] hello"_sv.asBytes()));
	bzd::log::warning(CSTR("hello")).sync();
	EXPECT_TRUE(backend.match("[w] [logger.cc:20] hello"_sv.asBytes()));
	bzd::log::info(CSTR("hello")).sync();
	EXPECT_TRUE(backend.match("[i] [logger.cc:22] hello"_sv.asBytes()));
	bzd::log::debug(CSTR("hello")).sync();
	EXPECT_TRUE(backend.match("[d] [logger.cc:24] hello"_sv.asBytes()));

	// Complex message
	bzd::log::error(CSTR("hello {}"), "world").sync();
	EXPECT_TRUE(backend.match("[e] [logger.cc:28] hello world"_sv.asBytes()));
	bzd::log::warning(CSTR("hello {}"), "world").sync();
	EXPECT_TRUE(backend.match("[w] [logger.cc:30] hello world"_sv.asBytes()));
	bzd::log::info(CSTR("hello {}"), "world").sync();
	EXPECT_TRUE(backend.match("[i] [logger.cc:32] hello world"_sv.asBytes()));
	bzd::log::debug(CSTR("hello {}"), "world").sync();
	EXPECT_TRUE(backend.match("[d] [logger.cc:34] hello world"_sv.asBytes()));
}
