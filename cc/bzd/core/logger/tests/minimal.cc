#include "cc/bzd/core/logger/minimal.hh"

#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/core/logger/tests/support/backend.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/scope_guard.hh"

TEST(Minimal, Empty)
{
	bzd::minimal::log::info("hello");
}

TEST(Minimal, Backend)
{
	static bzd::test::Logger<1024> backend;
	auto maybePrevious = bzd::backend::Logger::setDefault(backend);
	const bzd::ScopeGuard scope{[&maybePrevious]() {
		if (maybePrevious)
		{
			bzd::backend::Logger::setDefault(maybePrevious.valueMutable());
		}
	}};

	// Simple message
	bzd::minimal::log::error("hello");
	EXPECT_TRUE(backend.match("[e] [minimal.cc:25] hello\n"_sv.asBytes()));
	bzd::minimal::log::warning("hello");
	EXPECT_TRUE(backend.match("[w] [minimal.cc:27] hello\n"_sv.asBytes()));
	bzd::minimal::log::info("hello");
	EXPECT_TRUE(backend.match("[i] [minimal.cc:29] hello\n"_sv.asBytes()));

	// Default level is INFO, this will be hidden
	bzd::minimal::log::debug("hello");
	EXPECT_FALSE(backend.match("[d] [minimal.cc:33] hello\n"_sv.asBytes()));
}
