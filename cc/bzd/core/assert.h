#pragma once

namespace bzd::assert::impl {
inline void assertHelper(bool test)
{
	throw "Assert error";
}

void backend(const char* message1, const char* message2 = nullptr);

} // namespace bzd::assert::impl

namespace bzd::assert {

constexpr bool isTrueConstexpr(const bool condition)
{
	return (condition) ? true : (impl::assertHelper(condition), false);
}

constexpr void isTrue(const bool condition)
{
	if (!condition)
	{
		impl::backend("Assertion failed.\n");
	}
}

constexpr void isTrue(const bool condition, const char* message)
{
	if (!condition)
	{
		impl::backend("Assertion failed with message: ", message);
	}
}

void unreachable();

} // namespace bzd::assert
