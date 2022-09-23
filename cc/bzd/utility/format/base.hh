#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/utility/constexpr_for.hh"

namespace bzd::format::impl {

// ---- Error handlers ----

class ConstexprAssert
{
public:
	// NOLINTNEXTLINE(bugprone-exception-escape)
	static constexpr void onError(const bzd::StringView&) noexcept { bzd::assert::isTrueConstexpr(false); }
};

class RuntimeAssert
{
public:
	static constexpr void onError(const bzd::StringView& view) noexcept { bzd::assert::isTrue(false, view.data()); }
};

class NoAssert
{
public:
	static constexpr void onError(const bzd::StringView&) noexcept {}
};

// ---- Parser ----

struct ResultStaticString
{
	Bool isMetadata;
	StringView str;
};

/// Parse a static string and returns when there is data to be processed.
template <class Adapter>
constexpr ResultStaticString parseStaticString(bzd::StringView& format) noexcept
{
	Size offset = 0;
	while (offset < format.size())
	{
		const auto c = format[offset++];
		if (c != '{' && c != '}')
		{
			continue;
		}

		Adapter::assertTrue(offset < format.size(), "Unexpected static string termination");

		if (format[offset] == c)
		{
			const auto str = format.subStr(0, offset);
			format.removePrefix(offset + 1);
			return {false, str};
		}

		Adapter::assertTrue(c == '{', "'}' must be part of a pair '{...}' or doubled '}}'");

		--offset;
		break;
	}

	const auto str = format.subStr(0, offset);
	format.removePrefix(offset);
	return {!format.empty(), str};
}

template <class Adapter>
class Parser
{
public:
	constexpr Parser(bzd::StringView format) noexcept : iteratorBegin_{format} {}

	class Iterator
	{
	public:
		struct Result
		{
			const bzd::Optional<const typename Adapter::Metadata> metadata;
			const StringView& str;
		};

	public:
		constexpr Iterator(bzd::StringView format) noexcept : format_{format} { next(); }

		constexpr Iterator& operator++() noexcept
		{
			next();
			return *this;
		}

		constexpr operator bool() const noexcept { return end_; }

		constexpr Result operator*() const noexcept
		{
			return (result_.isMetadata) ? Result{metadata_, result_.str} : Result{nullopt, result_.str};
		}

	private:
		constexpr void next() noexcept
		{
			if (format_.empty())
			{
				end_ = true;
			}
			else
			{
				result_ = parseStaticString<Adapter>(format_);
				if (result_.isMetadata)
				{
					Adapter::assertTrue(format_.front() == '{', "Unexpected return state for parseStaticString");
					Adapter::assertTrue(format_.size() > 1, "Unexpected return state for parseStaticString");
					format_.removePrefix(1);
					metadata_ = Adapter::template parse<Adapter>(format_, autoIndex_++);
				}
			}
		}

	private:
		bzd::StringView format_;
		ResultStaticString result_{};
		typename Adapter::Metadata metadata_{};
		Size autoIndex_ = 0;
		bool end_ = false;
	};

	constexpr Iterator begin() const noexcept { return iteratorBegin_; }

	constexpr bool end() const noexcept { return true; }

private:
	const Iterator iteratorBegin_;
};

/// Adapter used for the current parsing operation.
/// Different adapters are used for compile timme or runtime operations.
template <class... Ts>
class Adapter : public Ts...
{
public:
	static constexpr void assertTrue(const bool condition, const bzd::StringView& message) noexcept
	{
		if (!condition)
		{
			Adapter::onError(message);
		}
	}
};

/// Check the format context.
///
/// Check the format context with the argument type, this to ensure type safety.
/// This function should only be used at compile time.
template <Size index, class Adapter, class MetadataList, class T>
constexpr void contextCheck(const MetadataList& metadataList, const T& tuple) noexcept
{
	if constexpr (index > 0)
	{
		using ValueType = typename T::template Get<index - 1>;

		Adapter::assertTrue(Adapter::template hasFormatter<ValueType> || Adapter::template hasFormatterWithMetadata<ValueType>,
							"Argument type is not supported, it must contain a valid formatter.");

		Adapter::template check<index, ValueType, Adapter>(metadataList, tuple);
		contextCheck<index - 1, Adapter>(metadataList, tuple);
	}
}

template <class Formatter, class Schema, class T>
constexpr bool contextValidate(const bzd::StringView& format, const T& tuple) noexcept
{
	using ConstexprAdapter = Adapter<ConstexprAssert, Formatter, Schema>;
	bzd::VectorConstexpr<typename Schema::Metadata, 128> metadataList{};
	Parser<ConstexprAdapter> parser{format};

	for (const auto& result : parser)
	{
		if (result.metadata.hasValue())
		{
			metadataList.pushBack(result.metadata.value());
		}
	}

	contextCheck<T::size(), ConstexprAdapter>(metadataList, tuple);
	return true;
}

template <class Adapter>
class Processor
{
public:
	using TransportType = typename Adapter::FormatterTransportType;

public:
	template <class... Args>
	static constexpr auto make(Args&&... args) noexcept
	{
		// Make the actual lambda
		const auto lambdas = bzd::makeTuple([&args](TransportType & transport, const typename Adapter::Metadata& metadata) -> auto{
			return Adapter::process(transport, args, metadata);
		}...);

		return ProcessorType<decltype(lambdas)>{lambdas};
	}

private:
	template <class Lambdas>
	class ProcessorType
	{
	public:
		constexpr ProcessorType(Lambdas& lambdas) noexcept : lambdas_{lambdas} {}

		constexpr void process(TransportType& transport, const typename Adapter::Metadata& metadata) const noexcept
		{
			const auto index = metadata.index;
			Size counter = 0;
			constexprForContainerInc(lambdas_, [&](auto lambda) {
				if (counter++ == index)
				{
					lambda(transport, metadata);
				}
			});
		}

	private:
		const Lambdas lambdas_;
	};
};

} // namespace bzd::format::impl
