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

struct DefaultMetadata
{
	using Metadata = bzd::StringView;
	template <class Adapter>
	static constexpr Metadata parse(bzd::StringView& pattern) noexcept
	{
		Size index = 0u;
		for (; index < pattern.size() && pattern[index] != '}'; ++index)
		{
		}
		const auto result = pattern.subStr(0u, index);
		pattern.removePrefix(index);
		return result;
	}
};

template <class Metadata, class Adapter, class Tuple, Size index = Tuple::size()>
constexpr Metadata parseMetadata(const Size metadataIndex, bzd::StringView& pattern) noexcept
{
	if constexpr (index > 0)
	{
		using ValueType = bzd::typeTraits::RemoveCVRef<typename Tuple::template Get<index - 1>>;
		if (metadataIndex == index - 1)
		{
			if constexpr (Adapter::template hasMetadata<ValueType>())
			{
				return Adapter::template Specialization<ValueType>::template parse<Adapter>(pattern);
			}
			else
			{
				return DefaultMetadata::template parse<Adapter>(pattern);
			}
		}

		return parseMetadata<Metadata, Adapter, Tuple, index - 1>(metadataIndex, pattern);
	}

	return {};
}

template <class Adapter, class T>
using Metadata = typename typeTraits::
	Conditional<Adapter::template hasMetadata<T>(), typename Adapter::template Specialization<T>, DefaultMetadata>::Metadata;

template <class Adapter, bzd::concepts::constexprStringView Pattern, class... Args>
constexpr auto parse() noexcept
{
	using Tuple = bzd::meta::Tuple<Args...>;
	using MetadataVariant = bzd::Variant<bzd::Monostate, Metadata<Adapter, Args>...>;

	struct Result : public ResultStaticString
	{
		Size index{};
		typename Adapter::Metadata metadata{};
		MetadataVariant metadatas{};
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
			results[index].index = parseIndex<Adapter>(pattern, autoIndex++);
			Adapter::assertTrue(pattern.size() > 0, "Replacement field pattern ended abruptly (after parseIndex)");
			Adapter::assertTrue(results[index].index < Tuple::size(),
								"The index of one for the field is greater than the number of argument provided.");
			auto copyPattern = pattern;
			results[index].metadatas = parseMetadata<MetadataVariant, Adapter, Tuple>(results[index].index, copyPattern);
			Adapter::assertTrue(!results[index].metadatas.template is<bzd::Monostate>(), "The metadata was not processed.");
			Adapter::assertTrue(copyPattern.size() > 0, "Replacement field pattern ended abruptly (after parseIndex)");
			Adapter::assertTrue(copyPattern.front() == '}', "Invalid format for replacement field, expecting '}'");
			copyPattern.removePrefix(1);
			Adapter::template parse<Adapter>(results[index].metadata, pattern);
		}
		++index;
	}

	// Ensure that all arguments have been taken into account within the pattern.
	for (Size index = 0; index < Tuple::size(); ++index)
	{
		bool usedAtLeastOnce = false;
		for (const auto& result : results)
		{
			usedAtLeastOnce |= (result.isMetadata && result.index == index);
		}
		Adapter::assertTrue(usedAtLeastOnce, "At least one argument is not being used by the formating string");
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
				Adapter::assertTrue(fragment.index < T::size(), "The index specified is greater than the number of arguments provided");
				if (fragment.index == index - 1)
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
		const auto lambdas =
			bzd::makeTuple([&args](Range & range, const auto& metadata) -> auto{ return Adapter::process(range, args, metadata); }...);

		return ProcessorType<decltype(lambdas)>{lambdas};
	}

private:
	template <class Lambdas>
	class ProcessorType
	{
	public:
		constexpr ProcessorType(Lambdas& lambdas) noexcept : lambdas_{lambdas} {}

		template <class Fragment>
		constexpr bzd::Optional<bzd::Size> process(Range& range, const Fragment& fragment) const noexcept
		{
			const auto index = fragment.index;
			Size counter = 0;
			bzd::Optional<bzd::Size> result{};
			constexprForContainerInc(lambdas_, [&](auto lambda) {
				if (counter++ == index)
				{
					result = lambda(range, fragment.metadata);
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
	constexpr auto context = parse<ConstexprAdapter, Pattern, Args...>();
	constexpr auto isValid = contextCheck<Range, decltype(tuple)::size(), ConstexprAdapter>(context, tuple);
	static_assert(isValid, "Compile-time string format check failed.");

	auto processor = Processor<Range, Adapter<RuntimeAssert, Formatter, Schema>>::make(args...);

	return bzd::makeTuple(bzd::move(context), bzd::move(processor));
}

} // namespace bzd::pattern::impl
