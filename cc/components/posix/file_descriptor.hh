#pragma once

#include "cc/bzd/utility/ownership.hh"

namespace bzd::platform::posix {

/// Class providing accessors to a file descriptor.
/// This doesn't mean it has control over the file descriptor itself.
class FileDescriptorAccessor : public Ownership<FileDescriptorAccessor>
{
protected:
	using NativeType = int;

public:
	/// Value representing an invalid file descriptor.
	static constexpr NativeType invalid{-1};

public:
	/// Default constructor that construct an invalid file descriptor.
	FileDescriptorAccessor() = default;

	/// Construct a file descriptor object a POSIX file descriptor.
	///
	/// \param fd The file descriptor to be used.
	constexpr explicit FileDescriptorAccessor(const NativeType fd) noexcept : native_{fd} {}

public:
	/// Get the native representation of a file descriptor.
	constexpr NativeType native() const noexcept { return native_; }

	/// Check if a file descriptor is valid.
	constexpr BoolType isValid() const noexcept { return native_ != invalid; }

protected:
	NativeType native_{invalid};
};

/// The file descriptor object, non owning, that can be passsed around.
using FileDescriptor = Borrowed<FileDescriptorAccessor>;

/// Class that owns the file descriptor in the sense that it owns
/// its memory but also has power to close it if needed.
class FileDescriptorOwner : public FileDescriptorAccessor
{
public:
	using FileDescriptorAccessor::NativeType;
	using FileDescriptorAccessor::FileDescriptorAccessor;

	constexpr FileDescriptorOwner& operator=(const NativeType fd) noexcept
	{
		if (this->native_ != this->invalid)
		{
			close();
		}
		native_ = fd;
		return *this;
	}

	/// Close the file descriptor.
	void close() noexcept;

	/// Close the file descriptor on destruction of this object.
	~FileDescriptorOwner() noexcept;
};

} // namespace bzd::platform::posix
