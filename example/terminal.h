#include "bzd.h"

class Terminal
{
	/*public:
		enum class Flag : bzd::UInt8Type
		{
			nonBlocking,
			whenFull,
			whenAtLeastOneChar
		};
	*/
public:
	bzd::Result<bzd::SizeType> write(const bzd::Span<const bzd::ByteType>& data);

	/**
	 * Block until at least one character is available.
	 */
	bzd::Result<bzd::SizeType> read(const bzd::Span<bzd::ByteType>& data);
	// bzd::Promise<bzd::SizeType> read2(const bzd::Span<bzd::ByteType>& data);
};
