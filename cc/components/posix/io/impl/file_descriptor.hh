#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::platform::posix {

/// The file descriptor object, non owning, that can be passsed around.
class FileDescriptor
{
protected:
	using NativeType = int;

public:
	/// Value representing an invalid file descriptor.
	static constexpr NativeType invalid{-1};

public:
	/// Default constructor that construct an invalid file descriptor.
	FileDescriptor() = default;

	/// Construct a file descriptor object a POSIX file descriptor.
	///
	/// \param fd The file descriptor to be used.
	constexpr explicit FileDescriptor(const NativeType fd) noexcept : native_{fd} {}

public:
	/// Get the native representation of a file descriptor.
	constexpr NativeType native() const noexcept { return native_; }

	/// Check if a file descriptor is valid.
	constexpr Bool isValid() const noexcept { return native_ != invalid; }

protected:
	NativeType native_{invalid};
};

/// Class that owns the file descriptor in the sense that it owns
/// its memory but also has power to close it if needed.
class FileDescriptorOwner : public FileDescriptor
{
public:
	using FileDescriptor::FileDescriptor;
	using FileDescriptor::NativeType;

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
