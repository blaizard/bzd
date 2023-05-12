#pragma once

#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/is_convertible.hh"
#include "cc/bzd/type_traits/is_pointer.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/to_string/base.hh"
#include "cc/bzd/utility/pattern/to_string/integral.hh"

namespace bzd {

template <class T>
requires(concepts::pointer<T> && !concepts::convertible<T, bzd::StringView>)
struct ToString<T>
{
	template <bzd::concepts::outputStreamRange Range>
	static constexpr bzd::Optional<bzd::Size> process(Range&& range, const T value) noexcept
	{
		using Metadata = bzd::ToString<bzd::UInt64>::Metadata;

		Metadata metadata{};
		metadata.format = Metadata::Format::hexadecimalLower;
		metadata.alternate = true;
		return bzd::toString(range, reinterpret_cast<bzd::UInt64>(value), metadata);
	}
};

} // namespace bzd
