#include "cc/bzd.hh"
/*
class Terminal
{
	public:
		enum class Flag : bzd::UInt8
		{
			nonBlocking,
			whenFull,
			whenAtLeastOneChar
		};
public:
	[[nodiscard]] bzd::Promise<bzd::Size> write(const bzd::Span<const bzd::Byte>& data) noexcept;
	[[nodiscard]] bzd::Promise<bzd::Size> write(const bzd::Span<bzd::Byte>& data) noexcept
	{
		return write(reinterpret_cast<const bzd::Span<const bzd::Byte>&>(data));
	}
	bzd::Promise<bzd::Size> write(bzd::Span<const bzd::Byte>&& data) noexcept = delete;
	bzd::Promise<bzd::Size> write(bzd::Span<bzd::Byte>&& data) noexcept = delete;

	/// Block until at least one character is available and return it immediately.
	[[nodiscard]] bzd::Promise<bzd::Size> read(const bzd::Span<bzd::Byte>& data) noexcept;
	bzd::Promise<bzd::Size> read(bzd::Span<bzd::Byte>&& data) noexcept = delete;
};
*/
