#include "cc/components/posix/io/impl/file_descriptor.hh"

#include <unistd.h>

namespace bzd::platform::posix {

void FileDescriptorOwner::reset() noexcept
{
	if (this->native_ != this->invalid)
	{
		::close(this->native_);
		this->native_ = this->invalid;
	}
}

FileDescriptorOwner::~FileDescriptorOwner() noexcept
{
	reset();
}

} // namespace bzd::platform::posix
