#pragma once

#include "cc/bzd/utility/pattern/to_string/integral.hh"

namespace bzd {

template <>
struct ToString<bzd::Byte> : public ToString<bzd::UInt8>
{
	using Metadata = bzd::ToString<bzd::UInt8>::Metadata;

	template <bzd::concepts::outputByteCopyableRange Range>
	static constexpr bzd::Optional<bzd::Size> process(Range&& range,
													  const bzd::Byte value,
													  const Metadata metadata = Metadata{Metadata::Sign::automatic,
																						 /*alternate*/ true,
																						 Metadata::Format::hexadecimalLower}) noexcept
	{
		return ToString<bzd::UInt8>::process(bzd::forward<Range>(range), static_cast<bzd::UInt8>(value), metadata);
	}
};

} // namespace bzd
