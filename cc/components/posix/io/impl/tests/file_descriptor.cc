#include "cc/components/posix/io/file_descriptor.hh"

#include "cc/bzd/test/test.hh"

TEST(FileDescriptor, basic)
{
	bzd::components::posix::FileDescriptor fd1{};
	EXPECT_FALSE(fd1.isValid());

	bzd::components::posix::FileDescriptor fd2{12};
	EXPECT_TRUE(fd2.isValid());
}

TEST(FileDescriptorOwner, basic)
{
	bzd::components::posix::FileDescriptorOwner fd1{};
	EXPECT_FALSE(fd1.isValid());
}
