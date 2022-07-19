#include "cc/components/posix/io/impl/file_descriptor.hh"

#include <iostream>
#include <unistd.h>

namespace bzd::platform::posix {

FileDescriptorOwner& FileDescriptorOwner::operator=(FileDescriptorOwner&& other) noexcept
{
	reset();
	this->native_ = other.native_;
	other.native_ = this->invalid;
	return *this;
}

FileDescriptorOwner& FileDescriptorOwner::operator=(const NativeType fd) noexcept
{
	reset();
	native_ = fd;
	return *this;
}

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
