#include "cc/components/posix/io/impl/file_descriptor.hh"

#include <unistd.h>

namespace bzd::platform::posix {

void FileDescriptorOwner::close() noexcept
{
    ::close(this->native_);
    this->native_ = this->invalid;
}

FileDescriptorOwner::~FileDescriptorOwner() noexcept
{
    if (this->native_ != this->invalid)
    {
        close();
    }
}

}
