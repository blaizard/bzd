#include "cc/components/posix/file_descriptor.hh"

#include "cc/bzd/test/test.hh"

TEST(FileDescriptor, basic)
{
    bzd::platform::posix::FileDescriptor fd{};
}
