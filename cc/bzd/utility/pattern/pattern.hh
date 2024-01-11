#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/utility/apply.hh"
#include "cc/bzd/utility/constexpr_for.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/pattern/from_string/integral.hh"

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
};

namespace concepts {

/// Checks if a specialization has metadata.
template <class Adapter, class T>
concept metadata = requires { typename Adapter::template Specialization<T>::Metadata; };

} // namespace concepts

template <class Adapter, class T>
using Metadata = typename typeTraits::
	Conditional<concepts::metadata<Adapter, T>, typename Adapter::template Specialization<T>, DefaultMetadata>::Metadata;

template <class Adapter, class... Args>
using Metadatas = bzd::Variant<bzd::Monostate, Metadata<Adapter, Args>...>;

template <class Adapter>
constexpr bzd::StringView parseOptions(bzd::StringView& pattern) noexcept
{
	Size index = 0u;
	for (; index < pattern.size() && pattern[index] != '}'; ++index)
	{
	}
	const auto result = pattern.subStr(0u, index);
	pattern.removePrefix(index);
	Adapter::assertTrue(pattern.size() > 0, "Replacement field pattern ended abruptly (after parseIndex)");
	Adapter::assertTrue(pattern.front() == '}', "Invalid format for replacement field, expecting '}'");
	pattern.removePrefix(1);
	return result;
}

template <class ReturnType, class Adapter, class T, class... Ts>
constexpr ReturnType parseMetadata(const Size metadataIndex, const bzd::StringView options, const Size index = 0u) noexcept
{
	if (metadataIndex == index)
	{
		if constexpr (concepts::metadata<Adapter, T>)
		{
			return Adapter::template Specialization<T>::template parse<Adapter>(options);
		}
		else
		{
			Adapter::assertTrue(options.empty(), "Pattern options are not supported for this type.");
			return options;
		}
	}

	if constexpr (sizeof...(Ts))
	{
		return parseMetadata<ReturnType, Adapter, Ts...>(metadataIndex, options, index + 1u);
	}

	return {};
}

template <class Adapter, bzd::concepts::constexprStringView Pattern, class... Args>
constexpr auto parse() noexcept
{
	struct Result : public ResultStaticString
	{
		Size index{};
		Metadatas<Adapter, Args...> metadatas{};
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
			Adapter::assertTrue(sizeof...(Args) >= 0 && results[index].index < sizeof...(Args),
								"The index of one for the field is greater than the number of argument provided.");
			if constexpr (sizeof...(Args))
			{
				const auto options = parseOptions<Adapter>(pattern);
				results[index].metadatas = parseMetadata<decltype(Result::metadatas), Adapter, Args...>(results[index].index, options);
				Adapter::assertTrue(!results[index].metadatas.template is<bzd::Monostate>(), "The metadata was not processed.");
			}
		}
		++index;
	}

	// Ensure that all arguments have been taken into account within the pattern.
	for (Size index = 0; index < sizeof...(Args); ++index)
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

template <class Range, class Adapter>
class Processor
{
public:
	template <class... Args>
	static constexpr auto make(Args&... args) noexcept
	{
		// Make a lambda that capture the arguments and cast the Metadata type into the correct one.
		const auto lambdas = bzd::makeTuple([&args](Range& range, const Metadatas<Adapter, Args...>& metadatas) -> auto {
			if constexpr (concepts::metadata<Adapter, Args>)
			{
				const auto& metadata = metadatas.template get<Metadata<Adapter, Args>>();
				return Adapter::template Specialization<Args>::process(range, args, metadata);
			}
			else
			{
				return Adapter::template Specialization<Args>::process(range, args);
			}
		}...);

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
					result = lambda(range, fragment.metadatas);
				}
			});
			return result;
		}

	private:
		const Lambdas lambdas_;
	};
};

template <class Range, class Schema, bzd::concepts::constexprStringView Pattern, class... Args>
constexpr auto make(const Pattern&, Args&&... args) noexcept
{
	constexpr auto context = parse<Adapter<ConstexprAssert, Schema>, Pattern, Args...>();
	static_assert(context.size() >= 0, "Compile-time string format check failed.");
	// TODO: remove the use of args here and propagate it at a later stage so that this function
	// can be consteval. This would make this code fully compile time and maybe better error message?
	auto processor = Processor<Range, Adapter<RuntimeAssert, Schema>>::make(args...);

	return bzd::makeTuple(bzd::move(context), bzd::move(processor));
}

} // namespace bzd::pattern::impl
