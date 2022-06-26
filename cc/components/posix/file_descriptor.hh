#pragma once

#include "cc/bzd/utility/owner.hh"

namespace bzd::platform::posix {

class FileDescriptor : public Owner<FileDescriptor>
{
private:
	using NativeType = int;

public:
	static constexpr NativeType invalid{-1};

public:
	/// Get the native representation of a file descriptor.
	constexpr NativeType native() const noexcept { return native_; }

	/// Check if a file descriptor is valid.
	constexpr BoolType isValid() const noexcept { return native_ != invalid; }

	/// Close the file descriptor.
	void close() noexcept;

private:
	NativeType native_{invalid};
};

} // namespace bzd::platform::posix
