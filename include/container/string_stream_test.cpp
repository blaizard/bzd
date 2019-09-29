#include "gtest/gtest.h"
#include "include/container/string_stream.h"

TEST(ContainerStringStream, Base)
{
    bzd::StringStream<10> sstreamCstor;
    bzd::interface::StringStream sstream = sstreamCstor;

	sstream.write("Hello");
	EXPECT_STREQ(sstream.str().data(), "Hello");
}
