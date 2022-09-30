#include "cc/bzd/core/logger.hh"

#include "cc/bzd/core/logger/tests/support/backend.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/scope_guard.hh"

TEST(Logger, Empty) { bzd::log::info("hello"_csv).sync(); }

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
	bzd::log::error("hello"_csv).sync();
	EXPECT_TRUE(backend.match("[e] [logger.cc:25] hello\n"_sv.asBytes()));
	bzd::log::warning("hello"_csv).sync();
	EXPECT_TRUE(backend.match("[w] [logger.cc:27] hello\n"_sv.asBytes()));
	bzd::log::info("hello"_csv).sync();
	EXPECT_TRUE(backend.match("[i] [logger.cc:29] hello\n"_sv.asBytes()));
	bzd::log::debug("hello"_csv).sync();
	EXPECT_TRUE(backend.match("[d] [logger.cc:31] hello\n"_sv.asBytes()));

	// Complex message
	bzd::log::error("hello {}"_csv, "world").sync();
	EXPECT_TRUE(backend.match("[e] [logger.cc:35] hello world\n"_sv.asBytes()));
	bzd::log::warning("hello {}"_csv, "world").sync();
	EXPECT_TRUE(backend.match("[w] [logger.cc:37] hello world\n"_sv.asBytes()));
	bzd::log::info("hello {}"_csv, "world").sync();
	EXPECT_TRUE(backend.match("[i] [logger.cc:39] hello world\n"_sv.asBytes()));
	bzd::log::debug("hello {}"_csv, "world").sync();
	EXPECT_TRUE(backend.match("[d] [logger.cc:41] hello world\n"_sv.asBytes()));
}
