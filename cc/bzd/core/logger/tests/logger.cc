#include "cc/bzd/core/logger.hh"

#include "cc/bzd/core/logger/tests/support/backend.hh"
#include "cc/bzd/utility/scope_guard.hh"
#include "cc_test/test.hh"

TEST(Logger, Empty)
{
	bzd::log::info(CSTR("hello")).sync();
}

TEST(Logger, Backend)
{
	static bzd::test::Logger<1024> backend;
	auto maybePrevious = bzd::backend::Logger::setDefault(backend);
	const bzd::ScopeGuard scope{[&maybePrevious]() {
		if (maybePrevious)
		{
			bzd::backend::Logger::setDefault(maybePrevious.valueMutable());
		}
	}};
	bzd::Logger::getDefault().setMinimumLevel(bzd::log::Level::DEBUG);

	// Simple message
	bzd::log::error(CSTR("hello")).sync();
	EXPECT_TRUE(backend.match("[e] [logger.cc:25] hello\n"_sv.asBytes()));
	bzd::log::warning(CSTR("hello")).sync();
	EXPECT_TRUE(backend.match("[w] [logger.cc:27] hello\n"_sv.asBytes()));
	bzd::log::info(CSTR("hello")).sync();
	EXPECT_TRUE(backend.match("[i] [logger.cc:29] hello\n"_sv.asBytes()));
	bzd::log::debug(CSTR("hello")).sync();
	EXPECT_TRUE(backend.match("[d] [logger.cc:31] hello\n"_sv.asBytes()));

	// Complex message
	bzd::log::error(CSTR("hello {}"), "world").sync();
	EXPECT_TRUE(backend.match("[e] [logger.cc:35] hello world\n"_sv.asBytes()));
	bzd::log::warning(CSTR("hello {}"), "world").sync();
	EXPECT_TRUE(backend.match("[w] [logger.cc:37] hello world\n"_sv.asBytes()));
	bzd::log::info(CSTR("hello {}"), "world").sync();
	EXPECT_TRUE(backend.match("[i] [logger.cc:39] hello world\n"_sv.asBytes()));
	bzd::log::debug(CSTR("hello {}"), "world").sync();
	EXPECT_TRUE(backend.match("[d] [logger.cc:41] hello world\n"_sv.asBytes()));
}
