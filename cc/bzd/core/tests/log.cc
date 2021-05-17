#include "bzd/core/log.h"

#include "cc_test/test.h"

TEST(Log, Base)
{
	bzd::Log log{bzd::platform::getOut()};
	log.info("Hello {}"_sv, "World");
}
