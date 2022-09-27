#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/meta/tuple.hh"
#include "cc/bzd/utility/constexpr_for.hh"

namespace bzd::pattern::impl {

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
constexpr ResultStaticString parseStaticString(bzd::StringView& pattern) noexcept
{
	Size offset = 0;
	while (offset < pattern.size())
	{
		const auto c = pattern[offset++];
		if (c != '{' && c != '}')
		{
			continue;
		}

		Adapter::assertTrue(offset < pattern.size(), "Unexpected static string termination");

		if (pattern[offset] == c)
		{
			const auto str = pattern.subStr(0, offset);
			pattern.removePrefix(offset + 1);
			return {false, str};
		}

		Adapter::assertTrue(c == '{', "'}' must be part of a pair '{...}' or doubled '}}'");

		--offset;
		break;
	}

	const auto str = pattern.subStr(0, offset);
	pattern.removePrefix(offset);
	return {!pattern.empty(), str};
}

template <class Adapter>
class Parser
{
public:
	constexpr Parser(bzd::StringView pattern) noexcept : iteratorBegin_{pattern} {}

	class Iterator
	{
	public:
		struct Result
		{
			const bzd::Optional<const typename Adapter::Metadata> metadata;
			const StringView& str;
		};

	public:
		constexpr Iterator(bzd::StringView pattern) noexcept : pattern_{pattern} { next(); }

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
			if (pattern_.empty())
			{
				end_ = true;
			}
			else
			{
				result_ = parseStaticString<Adapter>(pattern_);
				if (result_.isMetadata)
				{
					Adapter::assertTrue(pattern_.front() == '{', "Unexpected return state for parseStaticString");
					Adapter::assertTrue(pattern_.size() > 1, "Unexpected return state for parseStaticString");
					pattern_.removePrefix(1);
					// Look for the index
					metadata_ = typename Adapter::Metadata{};
					metadata_.index = parseIndex(pattern_, autoIndex_++);
					Adapter::assertTrue(pattern_.size() > 0, "Replacement field pattern ended abruptly (after parseIndex)");
					Adapter::template parse<Adapter>(metadata_, pattern_);
				}
			}
		}

	private:
		bzd::StringView pattern_;
		ResultStaticString result_{};
		typename Adapter::Metadata metadata_{};
		Size autoIndex_ = 0;
		bool end_ = false;
	};

	constexpr Iterator begin() const noexcept { return iteratorBegin_; }

	constexpr bool end() const noexcept { return true; }

private:
	/// Return the positional index
	static constexpr bzd::Size parseIndex(bzd::StringView& pattern, const bzd::Size autoIndex) noexcept
	{
		bzd::Size index = 0;
		const bool isDefined = parseUnsignedInteger(pattern, index);
		if (pattern.front() == ':')
		{
			pattern.removePrefix(1);
		}
		else
		{
			Adapter::assertTrue(pattern.front() == '}', "Expecting closing '}' for the replacement field");
		}
		return (isDefined) ? index : autoIndex;
	}

	/// TODO: use the implementation fromString
	/// Parse an unsigned integer
	static constexpr bool parseUnsignedInteger(bzd::StringView& pattern, bzd::Size& integer) noexcept
	{
		bool isDefined = false;
		integer = 0;
		for (; pattern.size() > 0 && pattern.front() >= '0' && pattern.front() <= '9';)
		{
			isDefined = true;
			integer = integer * 10 + (pattern.front() - '0');
			pattern.removePrefix(1);
		}
		return isDefined;
	}

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

template <class Adapter, class Range, class Value, class Metadata>
concept compatibleProcessor = requires(Range& range, Value& value, Metadata& metadata)
{
	Adapter::process(range, value, metadata);
};

/// Check the pattern context.
///
/// Check the pattern context with the argument type, this to ensure type safety.
/// This function should only be used at compile time.
template <class Range, Size index, class Adapter, class MetadataList, class T>
constexpr void contextCheck(const MetadataList& metadataList, const T& tuple) noexcept
{
	if constexpr (index > 0)
	{
		using ValueType = bzd::typeTraits::RemoveCVRef<typename T::template Get<index - 1>>;

		Adapter::assertTrue(compatibleProcessor<Adapter, Range, ValueType, typename Adapter::Metadata>,
							"This value type does not have any compatible processor.");

		bool usedAtLeastOnce = false;
		for (const auto& metadata : metadataList)
		{
			Adapter::assertTrue(metadata.index < T::size(), "The index specified is greater than the number of arguments provided");
			if (metadata.index == index - 1)
			{
				usedAtLeastOnce = true;
				Adapter::template check<Adapter, ValueType>(metadata);
			}
		}

		Adapter::assertTrue(usedAtLeastOnce, "At least one argument is not being used by the formating string");
		contextCheck<Range, index - 1, Adapter>(metadataList, tuple);
	}
}

template <class Range, class Formatter, class Schema, class T>
constexpr bool contextValidate(const bzd::StringView& pattern, const T& tuple) noexcept
{
	using ConstexprAdapter = Adapter<ConstexprAssert, Formatter, Schema>;
	bzd::VectorConstexpr<typename Schema::Metadata, 128> metadataList{};
	Parser<ConstexprAdapter> parser{pattern};

	for (const auto& result : parser)
	{
		if (result.metadata.hasValue())
		{
			metadataList.pushBack(result.metadata.value());
		}
	}

	contextCheck<Range, T::size(), ConstexprAdapter>(metadataList, tuple);
	return true;
}

template <class Range, class Adapter>
class Processor
{
public:
	template <class... Args>
	static constexpr auto make(Args&... args) noexcept
	{
		// Make the actual lambda
		const auto lambdas = bzd::makeTuple([&args](Range & range, const typename Adapter::Metadata& metadata) -> auto{
			return Adapter::process(range, args, metadata);
		}...);

		return ProcessorType<decltype(lambdas)>{lambdas};
	}

private:
	template <class Lambdas>
	class ProcessorType
	{
	public:
		constexpr ProcessorType(Lambdas& lambdas) noexcept : lambdas_{lambdas} {}

		constexpr void process(Range& range, const typename Adapter::Metadata& metadata) const noexcept
		{
			const auto index = metadata.index;
			Size counter = 0;
			constexprForContainerInc(lambdas_, [&](auto lambda) {
				if (counter++ == index)
				{
					lambda(range, metadata);
				}
			});
		}

	private:
		const Lambdas lambdas_;
	};
};

template <class Range, class Formatter, class Schema, bzd::concepts::constexprStringView Pattern, class... Args>
constexpr auto make(const Pattern&, Args&&... args) noexcept
{
	// Compile-time pattern check
	constexpr const bzd::meta::Tuple<Args...> tuple{};
	constexpr const bool isValid = contextValidate<Range, Formatter, Schema>(Pattern::value(), tuple);
	// This line enforces compilation time evaluation
	static_assert(isValid, "Compile-time string format check failed.");

	constexpr Parser<Adapter<NoAssert, Formatter, Schema>> parser{Pattern::value()};
	auto processor = Processor<Range, Adapter<RuntimeAssert, Formatter, Schema>>::make(args...);

	return bzd::makeTuple(bzd::move(parser), bzd::move(processor));
}

} // namespace bzd::pattern::impl
