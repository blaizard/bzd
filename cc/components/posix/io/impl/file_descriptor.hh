#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::components::posix {

/// The file descriptor object, non owning, that can be passed around.
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

	FileDescriptorOwner(const FileDescriptorOwner&&) = delete;
	FileDescriptorOwner& operator=(const FileDescriptorOwner&) = delete;
	constexpr FileDescriptorOwner(FileDescriptorOwner&& other) noexcept : FileDescriptor{other.native_} { other.native_ = this->invalid; }
	FileDescriptorOwner& operator=(FileDescriptorOwner&& other) noexcept;
	/// Close the file descriptor on destruction of this object.
	~FileDescriptorOwner() noexcept;

	FileDescriptorOwner& operator=(const NativeType fd) noexcept;

	/// Close the file descriptor.
	void reset() noexcept;
};

} // namespace bzd::components::posix
