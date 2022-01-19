#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/variant.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/format/format.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/source_location.hh"

namespace bzd {
enum class ErrorType : bzd::UInt8Type
{
	/// Any error that do not fall in the below categories.
	failure,
	/// Happens when a timeout occurs, it could provide information to the handler
	/// that either the timeout was too short or the input was too slow.
	timeout,
	/// Happens while accessing a busy ressource for example,
	/// this type of error notifies the handler that it can be retried if necessary.
	busy
};

/// Global definition of an error object.
///
/// This is a lightwheight data structure that combines the following:
/// - An abstracted information regarding the error type on which
///   the error handler could react.
/// - The error location to trace the error for debugging purpose.
/// - A human readable message that describes details of the actual
///   error, which could include for example, implementation specific information.
class [[nodiscard]] Error
{
public: // Traits.
	using LineType = SourceLocation::LineType;
	using Self = Error;

public:
	template <class T, REQUIRES(bzd::isConstexprStringView<T>)>
	constexpr Error(const SourceLocation& location, const ErrorType type, const T) noexcept :
		source_{location.getFileName()}, line_{location.getLine()}, type_{type}, message_{inPlaceIndex<0>, T::value()}
	{
	}

	template <class... Args>
	constexpr Error(const SourceLocation& location, const ErrorType type, Args&&... args) noexcept :
		source_{location.getFileName()}, line_{location.getLine()}, type_{type}, message_{inPlaceIndex<1>, reserveBuffer()}
	{
		auto& maybeString = message_.template get<Optional<interface::String&>>();
		if (maybeString)
		{
			::toString(maybeString.valueMutable(), bzd::forward<Args>(args)...);
		}
	}

	constexpr Error(const Self&) noexcept = delete;
	constexpr Self& operator=(const Self&) noexcept = delete;

	// Only move constructor is allowed.
	constexpr Error(Self&& other) noexcept : source_{other.source_}, line_{other.line_}, type_{other.type_}
	{
		other.message_.match(
			[this](Optional<interface::String&>& value) {
				message_.emplace<Optional<interface::String&>>(bzd::move(value));
				value.reset();
			},
			[this](StringView& value) { message_.emplace<StringView>(bzd::move(value)); });
	}
	constexpr Self& operator=(Self&& other) noexcept = delete;

	~Error() noexcept;

public: // Accessors.
	[[nodiscard]] constexpr StringView getSource() const noexcept { return source_; }
	[[nodiscard]] constexpr LineType getLine() const noexcept { return line_; }
	[[nodiscard]] constexpr ErrorType getType() const noexcept { return type_; }
	[[nodiscard]] constexpr StringView getTypeAsString() const noexcept
	{
		switch (type_)
		{
		case ErrorType::failure:
			return "failure"_sv;
		case ErrorType::timeout:
			return "timeout"_sv;
		case ErrorType::busy:
			return "busy"_sv;
		}
		return "unknown"_sv;
	}
	[[nodiscard]] constexpr StringView getMessage() const noexcept
	{
		if (message_.template is<StringView>())
		{
			return message_.template get<StringView>();
		}
		auto& maybeString = message_.template get<Optional<interface::String&>>();
		if (maybeString)
		{
			return maybeString.value().asSpan();
		}
		return ""_sv;
	}

private:
	static Optional<interface::String&> reserveBuffer() noexcept;

private:
	StringView source_;
	LineType line_;
	ErrorType type_;
	Variant<StringView, Optional<interface::String&>> message_;
};

template <class A>
constexpr auto error(const ErrorType type, A&& a, const SourceLocation location = SourceLocation::current()) noexcept
{
	return bzd::error(Error{location, type, bzd::forward<A>(a)});
}
template <class A, class B>
constexpr auto error(const ErrorType type, A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
{
	return bzd::error(Error{location, type, bzd::forward<A>(a), bzd::forward<B>(b)});
}
template <class A, class B, class C>
constexpr auto error(const ErrorType type, A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
{
	return bzd::error(Error{location, type, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c)});
}
template <class A, class B, class C, class D>
constexpr auto error(const ErrorType type, A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
{
	return bzd::error(Error{location, type, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d)});
}
template <class A, class B, class C, class D, class E>
constexpr auto error(
	const ErrorType type, A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
{
	return bzd::error(
		Error{location, type, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), bzd::forward<E>(e)});
}
template <class A, class B, class C, class D, class E, class F>
constexpr auto error(
	const ErrorType type, A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
{
	return bzd::error(Error{location,
							type,
							bzd::forward<A>(a),
							bzd::forward<B>(b),
							bzd::forward<C>(c),
							bzd::forward<D>(d),
							bzd::forward<E>(e),
							bzd::forward<F>(f)});
}
template <class A, class B, class C, class D, class E, class F, class G>
constexpr auto error(const ErrorType type,
					 A&& a,
					 B&& b,
					 C&& c,
					 D&& d,
					 E&& e,
					 F&& f,
					 G&& g,
					 const SourceLocation location = SourceLocation::current()) noexcept
{
	return bzd::error(Error{location,
							type,
							bzd::forward<A>(a),
							bzd::forward<B>(b),
							bzd::forward<C>(c),
							bzd::forward<D>(d),
							bzd::forward<E>(e),
							bzd::forward<F>(f),
							bzd::forward<G>(g)});
}

} // namespace bzd

/// Helper to propagate an error on an asynchronous call.
/// After this call, it asserts that the result contains no error.
#define ASSERT_ASYNC_RESULT(result)    \
	if (!(result))                     \
	{                                  \
		co_return(result).propagate(); \
	}
