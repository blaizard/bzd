#include "cc/bzd/core/logger/logger.h"

#include "cc/bzd/core/logger/tests/support/backend.h"
#include "cc_test/test.h"
/*
TEST(Logger, Empty)
{
	bzd::log::info(CSTR("hello")).sync();
}
*/
TEST(Logger, Backend)
{
	bzd::test::Logger<1024> backend;
	bzd::backend::Logger::setDefault(backend);

	// Simple message
	bzd::log::info(CSTR("hello")).sync();
	EXPECT_TRUE(backend.match("[i] hello"_sv.asBytes()));

	// Complex message
	bzd::log::info(CSTR("hello {}"), "world").sync();
	EXPECT_TRUE(backend.match("[i] hello world"_sv.asBytes()));
}
