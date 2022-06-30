#include "cc/components/posix/io/file_descriptor.hh"

#include "cc/bzd/test/test.hh"

TEST(FileDescriptor, basic)
{
	bzd::platform::posix::FileDescriptor fd1{};
	EXPECT_FALSE(fd1.isValid());

	bzd::platform::posix::FileDescriptor fd2{12};
	EXPECT_TRUE(fd2.isValid());
}

TEST(FileDescriptorOwner, basic)
{
	bzd::platform::posix::FileDescriptorOwner fd1{};
	EXPECT_FALSE(fd1.isValid());

	bzd::platform::posix::FileDescriptorOwner fd2{12};
	EXPECT_TRUE(fd2.isValid());
}
