#include "cc/components/posix/file_descriptor.hh"

#include "cc/bzd/test/test.hh"

TEST(FileDescriptorAccessor, basic)
{
	bzd::platform::posix::FileDescriptorAccessor fd1{};
	EXPECT_FALSE(fd1.isValid());

	bzd::platform::posix::FileDescriptorAccessor fd2{12};
	EXPECT_TRUE(fd2.isValid());
}

TEST(FileDescriptorOwner, basic)
{
	bzd::platform::posix::FileDescriptorOwner fd1{};
	EXPECT_FALSE(fd1.isValid());

	bzd::platform::posix::FileDescriptorOwner fd2{12};
	EXPECT_TRUE(fd2.isValid());
}
