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

public:
	template <class T, bzd::typeTraits::EnableIf<!HasFormatterWithMetadata<StreamFormatter, T>::value, void>* = nullptr>
	static Async<> process(bzd::OStream& stream, const T& value, const Metadata&) noexcept
	{
		co_await toStream(stream, value);
		co_return {};
	}

	template <class T, bzd::typeTraits::EnableIf<HasFormatterWithMetadata<StreamFormatter, T>::value, void>* = nullptr>
	static Async<> process(bzd::OStream& stream, const T& value, const Metadata& metadata) noexcept
	{
		co_await toStream(stream, value, metadata);
		co_return {};
	}
};

// Specialization of toStream for the native types

template <class T,
		  bzd::typeTraits::EnableIf<bzd::typeTraits::isIntegral<T> || bzd::typeTraits::isFloatingPoint<T> ||
										(bzd::typeTraits::isPointer<T> && !bzd::typeTraits::isConstructible<bzd::StringView, T>),
									void>* = nullptr>
Async<> toStream(bzd::OStream& stream, const T& value, const Metadata& metadata)
{
	bzd::String<80> str{};
	toString(str, value, metadata);
	co_await stream.write(str.asBytes());
	co_return {};
}

inline Async<> toStream(bzd::OStream& stream, const bzd::StringView stringView, const Metadata& metadata)
{
	co_await stream.write(processCommon(stringView, metadata).asBytes());
	co_return {};
}

template <class Adapter>
class FormatterAsync
{
public:
	using TransportType = typename Adapter::FormatterTransportType;

public:
	template <class... Args>
	static constexpr auto make(Args&... args) noexcept
	{
		// Make the actual lambda
		const auto lambdas = bzd::makeTuple([&args](TransportType& transport, const Metadata& metadata) -> bzd::Async<> {
			co_await Adapter::process(transport, args, metadata);
			co_return {};
		}...);

		return FormatterType<decltype(lambdas)>{lambdas};
	}

private:
	template <class Lambdas>
	class FormatterType
	{
	public:
		constexpr FormatterType(Lambdas& lambdas) noexcept : lambdas_{lambdas} {}

		bzd::Async<> process(TransportType& transport, const Metadata& metadata) const noexcept
		{
			const auto index = metadata.index;
			if constexpr (Lambdas::size() > 0)
			{
				if (index == 0)
				{
					co_await lambdas_.template get<0>()(transport, metadata);
				}
			}
			if constexpr (Lambdas::size() > 1)
			{
				if (index == 1)
				{
					co_await lambdas_.template get<1>()(transport, metadata);
				}
			}
			if constexpr (Lambdas::size() > 2)
			{
				if (index == 2)
				{
					co_await lambdas_.template get<2>()(transport, metadata);
				}
			}
			if constexpr (Lambdas::size() > 3)
			{
				if (index == 3)
				{
					co_await lambdas_.template get<3>()(transport, metadata);
				}
			}
			if constexpr (Lambdas::size() > 4)
			{
				if (index == 4)
				{
					co_await lambdas_.template get<4>()(transport, metadata);
				}
			}
			if constexpr (Lambdas::size() > 5)
			{
				if (index == 5)
				{
					co_await lambdas_.template get<5>()(transport, metadata);
				}
			}
			if constexpr (Lambdas::size() > 6)
			{
				if (index == 6)
				{
					co_await lambdas_.template get<6>()(transport, metadata);
				}
			}
			if constexpr (Lambdas::size() > 7)
			{
				if (index == 7)
				{
					co_await lambdas_.template get<7>()(transport, metadata);
				}
			}
			if constexpr (Lambdas::size() > 8)
			{
				if (index == 8)
				{
					co_await lambdas_.template get<8>()(transport, metadata);
				}
			}
			if constexpr (Lambdas::size() > 9)
			{
				if (index == 9)
				{
					co_await lambdas_.template get<9>()(transport, metadata);
				}
			}
			static_assert(Lambdas::size() <= 10, "Too many arguments passed to format, not supported.");

			co_return {};
		}

	private:
		const Lambdas lambdas_;
	};
};

} // namespace bzd::format::impl

// Note, Args&& here doesn't work for esp32 gcc compiler, the values are garbage, it seems that the temporary
// is out of scope within the coroutine. I remember seeing a bugzilla about this for gcc.
// Would be worth trying again with a new version of the compiler.
// TODO: Try to enable Args&& with an updated esp32 gcc compiler
template <bzd::concepts::constexprStringView T, class... Args>
bzd::Async<> toStream(bzd::OStream& stream, const T&, Args... args)
{
	// Compile-time format check
	constexpr const bzd::meta::Tuple<Args...> tuple{};
	constexpr const bool isValid = bzd::format::impl::contextValidate<bzd::format::impl::StreamFormatter>(T::value(), tuple);
	// This line enforces compilation time evaluation
	static_assert(isValid, "Compile-time string format check failed.");

	const auto formatter = bzd::format::impl::FormatterAsync<
		bzd::format::impl::Adapter<bzd::format::impl::RuntimeAssert, bzd::format::impl::StreamFormatter>>::make(args...);
	constexpr bzd::format::impl::Parser<bzd::format::impl::Adapter<bzd::format::impl::NoAssert, bzd::format::impl::StreamFormatter>> parser{
		T::value()};

	// Run-time call
	for (const auto& result : parser)
	{
		if (!result.str.empty())
		{
			co_await stream.write(result.str.asBytes());
		}
		if (result.metadata.hasValue())
		{
			co_await formatter.process(stream, result.metadata.value());
		}
	}

	co_return {};
}

inline bzd::Async<> toStream(bzd::OStream& stream, const bzd::interface::String& str)
{
	co_await stream.write(str.asBytes()).assert();
	co_return {};
}

inline bzd::Async<> toStream(bzd::OStream& stream, const bzd::StringView str)
{
	co_await stream.write(str.asBytes()).assert();
	co_return {};
}

inline bzd::Async<> toStream(bzd::OStream& stream, const char* const str)
{
	co_await stream.write(bzd::StringView{str}.asBytes()).assert();
	co_return {};
}
