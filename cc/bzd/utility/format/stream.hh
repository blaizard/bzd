#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/utility/format/format.hh"

namespace bzd::format::impl {

class StreamFormatter
{
public:
	template <class T>
	using FormatterType = decltype(toStream(bzd::typeTraits::declval<bzd::OStream&>(), bzd::typeTraits::declval<T>()));

	template <class T>
	using FormatterWithMetadataType = decltype(toStream(
		bzd::typeTraits::declval<bzd::OStream&>(), bzd::typeTraits::declval<T>(), bzd::typeTraits::declval<const Metadata>()));

	using FormatterTransportType = bzd::OStream;
	using FormatterReturnType = Async<void>;

public:
	template <class T, bzd::typeTraits::EnableIf<!HasFormatterWithMetadata<StreamFormatter, T>::value, void>* = nullptr>
	static Async<void> process(bzd::OStream& stream, const T& value, const Metadata&) noexcept
	{
		co_await toStream(stream, value);
	}

	template <class T, bzd::typeTraits::EnableIf<HasFormatterWithMetadata<StreamFormatter, T>::value, void>* = nullptr>
	static Async<void> process(bzd::OStream& stream, const T& value, const Metadata& metadata) noexcept
	{
		co_await toStream(stream, value, metadata);
	}
};

// Specialization of toStream for the native types

template <class T,
		  bzd::typeTraits::EnableIf<bzd::typeTraits::isIntegral<T> || bzd::typeTraits::isFloatingPoint<T> ||
										(bzd::typeTraits::isPointer<T> && !bzd::typeTraits::isConstructible<bzd::StringView, T>),
									void>* = nullptr>
Async<void> toStream(bzd::OStream& stream, const T& value, const Metadata& metadata)
{
	bzd::String<80> str{};
	toString(str, value, metadata);
	co_await stream.write(str.asBytes());
}

inline Async<void> toStream(bzd::OStream& stream, const bzd::StringView stringView, const Metadata& metadata)
{
	co_await stream.write(processCommon(stringView, metadata).asBytes());
}

}

namespace bzd::format {

template <class ConstexprStringView, class... Args, typename typeTraits::EnableIf<typeTraits::isBaseOf<bzd::ConstexprStringView, ConstexprStringView>, void*> = nullptr>
Async<void> toStream(bzd::OStream& stream, const ConstexprStringView&, Args&&... args)
{
	// Compile-time format check
	constexpr const bzd::Tuple<bzd::typeTraits::Decay<Args>...> tuple{};
	constexpr const bool isValid = bzd::format::impl::contextValidate<impl::StreamFormatter>(ConstexprStringView::value(), tuple);
	// This line enforces compilation time evaluation
	static_assert(isValid, "Compile-time string format check failed.");

	const auto formatter = impl::Formatter<impl::Adapter<impl::RuntimeAssert, impl::StreamFormatter>>::make(bzd::forward<Args>(args)...);
	constexpr impl::Parser<impl::Adapter<impl::NoAssert, impl::StreamFormatter>> parser{ConstexprStringView::value()};

	// Run-time call
	for (const auto& result : parser)
	{
		if (!result.str.empty())
		{
			co_await stream.write(result.str.asBytes());
		}
		if (result.metadata.hasValue())
		{
			co_await formatter.processAsync(stream, result.metadata.value());
		}
	}
}

inline Async<void> toStream(bzd::OStream& stream, const StringView& str)
{
	co_await stream.write(str.asBytes());
}

}
