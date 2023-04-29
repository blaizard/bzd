#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/meta/tuple.hh"
#include "cc/bzd/utility/constexpr_for.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/pattern/reader/integral.hh"

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
///
/// Returns isMetadata = true when it reaches a single '{' character.
/// Double '{{' or '}}' are returned as a unique character.
/// Returns the string parsed until then.
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

/// Compute the number of elements needed in the context to parse the full pattern.
template <class Adapter>
constexpr Size parseSize(bzd::StringView pattern) noexcept
{
	Size size = 0u;
	while (!pattern.empty())
	{
		const auto [isMetadata, _] = parseStaticString<Adapter>(pattern);
		if (isMetadata)
		{
			// Consume the metadata.
			const auto index = pattern.find('}');
			Adapter::assertTrue(index != bzd::npos, "Expecting closing '}' for the replacement field");
			pattern.removePrefix(index + 1u);
		}
		++size;
	}
	return size;
}

template <class Adapter>
constexpr bzd::Size parseIndex(bzd::StringView& pattern, const bzd::Size autoIndex) noexcept
{
	bzd::Size index = 0;
	const auto maybeIndex = bzd::fromString(pattern, index);
	if (maybeIndex)
	{
		pattern.removePrefix(maybeIndex.value());
	}
	if (pattern.front() == ':')
	{
		pattern.removePrefix(1);
	}
	else
	{
		Adapter::assertTrue(pattern.front() == '}', "Expecting closing '}' for the replacement field");
	}
	return (maybeIndex) ? index : autoIndex;
}

template <class Adapter, bzd::concepts::constexprStringView Pattern>
constexpr auto parse() noexcept
{
	struct Result : public ResultStaticString
	{
		typename Adapter::Metadata metadata{};
	};

	// Calculate the size and make the output array.
	constexpr Size size = parseSize<Adapter>(Pattern::value());
	bzd::Array<Result, size> results;

	// Parse the pattern.
	bzd::StringView pattern = Pattern::value();
	Size index = 0u;
	Size autoIndex = 0u;
	while (!pattern.empty())
	{
		static_cast<ResultStaticString&>(results[index]) = parseStaticString<Adapter>(pattern);
		if (results[index].isMetadata)
		{
			Adapter::assertTrue(pattern.front() == '{', "Unexpected return state for parseStaticString");
			Adapter::assertTrue(pattern.size() > 1, "Unexpected return state for parseStaticString");
			pattern.removePrefix(1);
			// Look for the index
			results[index].metadata.index = parseIndex<Adapter>(pattern, autoIndex++);
			Adapter::assertTrue(pattern.size() > 0, "Replacement field pattern ended abruptly (after parseIndex)");
			Adapter::template parse<Adapter>(results[index].metadata, pattern);
		}
		++index;
	}

	return results;
}

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
concept compatibleProcessor = requires(Range& range, Value& value, Metadata& metadata) { Adapter::process(range, value, metadata); };

/// Check the pattern context.
///
/// Check the pattern context with the argument type, this to ensure type safety.
/// This function should only be used at compile time.
template <class Range, Size index, class Adapter, class Context, class T>
constexpr bool contextCheck(const Context& context, const T& tuple) noexcept
{
	if constexpr (index > 0)
	{
		using ValueType = bzd::typeTraits::RemoveCVRef<typename T::template Get<index - 1>>;

		Adapter::assertTrue(compatibleProcessor<Adapter, Range, ValueType, typename Adapter::Metadata>,
							"This value type does not have any compatible processor.");

		bool usedAtLeastOnce = false;
		for (const auto& fragment : context)
		{
			if (fragment.isMetadata)
			{
				Adapter::assertTrue(fragment.metadata.index < T::size(),
									"The index specified is greater than the number of arguments provided");
				if (fragment.metadata.index == index - 1)
				{
					usedAtLeastOnce = true;
					Adapter::template check<Adapter, ValueType>(fragment.metadata);
				}
			}
		}

		Adapter::assertTrue(usedAtLeastOnce, "At least one argument is not being used by the formating string");
		bzd::ignore = contextCheck<Range, index - 1, Adapter>(context, tuple);
	}

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

		constexpr bzd::Optional<bzd::Size> process(Range& range, const typename Adapter::Metadata& metadata) const noexcept
		{
			const auto index = metadata.index;
			Size counter = 0;
			bzd::Optional<bzd::Size> result{};
			constexprForContainerInc(lambdas_, [&](auto lambda) {
				if (counter++ == index)
				{
					result = lambda(range, metadata);
				}
			});
			return result;
		}

	private:
		const Lambdas lambdas_;
	};
};

template <class Range, class Formatter, class Schema, bzd::concepts::constexprStringView Pattern, class... Args>
constexpr auto make(const Pattern&, Args&&... args) noexcept
{
	using ConstexprAdapter = Adapter<ConstexprAssert, Formatter, Schema>;

	constexpr const bzd::meta::Tuple<Args...> tuple{};
	constexpr auto context = parse<ConstexprAdapter, Pattern>();
	constexpr auto isValid = contextCheck<Range, decltype(tuple)::size(), ConstexprAdapter>(context, tuple);
	static_assert(isValid, "Compile-time string format check failed.");

	auto processor = Processor<Range, Adapter<RuntimeAssert, Formatter, Schema>>::make(args...);

	return bzd::makeTuple(bzd::move(context), bzd::move(processor));
}

} // namespace bzd::pattern::impl
