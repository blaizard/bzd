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
	[[nodiscard]] bzd::Promise<bzd::SizeType> write(const bzd::Span<const bzd::ByteType>& data) noexcept;

	/**
	 * Block until at least one character is available and return it immediatly.
	 */
	[[nodiscard]] bzd::Promise<bzd::SizeType> read(const bzd::Span<bzd::ByteType>& data) noexcept;

	template <class T>
	constexpr void write(const T&&) noexcept
	{
		static_assert(!bzd::typeTraits::isSame<T, T>, "Not allowed to pass an rvalue.");
	}

	template <class T>
	constexpr void read(const T&&) noexcept
	{
		static_assert(!bzd::typeTraits::isSame<T, T>, "Not allowed to pass an rvalue.");
	}
};
