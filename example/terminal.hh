#include "cc/bzd.hh"
/*
class Terminal
{
	public:
		enum class Flag : bzd::UInt8Type
		{
			nonBlocking,
			whenFull,
			whenAtLeastOneChar
		};
public:
	[[nodiscard]] bzd::Promise<bzd::SizeType> write(const bzd::Span<const bzd::ByteType>& data) noexcept;
	[[nodiscard]] bzd::Promise<bzd::SizeType> write(const bzd::Span<bzd::ByteType>& data) noexcept
	{
		return write(reinterpret_cast<const bzd::Span<const bzd::ByteType>&>(data));
	}
	bzd::Promise<bzd::SizeType> write(bzd::Span<const bzd::ByteType>&& data) noexcept = delete;
	bzd::Promise<bzd::SizeType> write(bzd::Span<bzd::ByteType>&& data) noexcept = delete;

	/// Block until at least one character is available and return it immediatly.
	[[nodiscard]] bzd::Promise<bzd::SizeType> read(const bzd::Span<bzd::ByteType>& data) noexcept;
	bzd::Promise<bzd::SizeType> read(bzd::Span<bzd::ByteType>&& data) noexcept = delete;
};
*/
