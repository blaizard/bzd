#pragma once

#include "cc/bzd/utility/format/format.hh"
/*
template <bzd::concepts::constexprStringView T, class... Args>
constexpr void fromString(bzd::StringView str, const T&, Args&&... args) noexcept
{
	// Compile-time format check
	constexpr const bzd::meta::Tuple<Args...> tuple{};
	constexpr const bool isValid = bzd::format::impl::contextValidate<bzd::format::impl::StringFormatter>(T::value(), tuple);
	// This line enforces compilation time evaluation
	static_assert(isValid, "Compile-time string format check failed.");

	const auto formatter =
		bzd::format::impl::Formatter<bzd::format::impl::Adapter<bzd::format::impl::RuntimeAssert,
																bzd::format::impl::StringFormatter>>::make(bzd::forward<Args>(args)...);
	constexpr bzd::format::impl::Parser<bzd::format::impl::Adapter<bzd::format::impl::NoAssert, bzd::format::impl::StringFormatter>> parser{
		T::value()};

	// Run-time call
	for (const auto& result : parser)
	{
		if (!result.str.empty())
		{
			str.append(result.str);
		}
		if (result.metadata.hasValue())
		{
			formatter.process(str, result.metadata.value());
		}
	}
}
*/
