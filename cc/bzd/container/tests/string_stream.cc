#include "cc/bzd/container/string_stream.hh"

#include "cc_test/test.hh"

TEST(ContainerStringStream, Base)
{
	bzd::StringStream<10> sstreamCstor;
	bzd::interface::StringStream& stream = sstreamCstor;

	stream.write("Hello"_sv.asBytes()).sync();
	EXPECT_STREQ(stream.str().data(), "Hello");
}
