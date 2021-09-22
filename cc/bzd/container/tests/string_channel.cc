#include "cc/bzd/container/string_channel.h"

#include "cc_test/test.h"

TEST(ContainerStringChannel, Base)
{
	bzd::StringChannel<10> sstreamCstor;
	bzd::interface::StringChannel& stream = sstreamCstor;

	stream.write("Hello"_sv.asBytes()).sync();
	EXPECT_STREQ(stream.str().data(), "Hello");
}
