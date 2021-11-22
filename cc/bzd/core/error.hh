#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/variant.hh"
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

public:
	template <class ConstString>
	constexpr Error(const SourceLocation& location, const ErrorType type, const ConstString) noexcept :
		source_{location.getFileName()}, line_{location.getLine()}, type_{type}, message_{inPlaceIndex<0>, ConstString::value()}
	{
	}

	template <class... Args>
	constexpr Error(const SourceLocation& location, const ErrorType type, Args&&... args) noexcept :
		source_{location.getFileName()}, line_{location.getLine()}, type_{type}, message_{inPlaceIndex<1>, reserveBuffer()}
	{
		auto& maybeString = message_.template get<Optional<interface::String&>>();
		if (maybeString)
		{
			format::toString(maybeString.valueMutable(), bzd::forward<Args>(args)...);
		}
	}

	~Error() noexcept;

public: // Accessors.
	[[nodiscard]] StringView getSource() const noexcept { return source_; }
	[[nodiscard]] LineType getLine() const noexcept { return line_; }
	[[nodiscard]] ErrorType getType() const noexcept { return type_; }
	[[nodiscard]] StringView getMessage() const noexcept
	{
		if (message_.template is<const StringView>())
		{
			return message_.template get<const StringView>();
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
	Variant<const StringView, Optional<interface::String&>> message_;
};

// void error() {}
} // namespace bzd

/// Helper to propagate an error on an asynchronous call.
/// After this call, it asserts that the result contains no error.
#define ASSERT_ASYNC_RESULT(result)    \
	if (!(result))                     \
	{                                  \
		co_return(result).propagate(); \
	}
