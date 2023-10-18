#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/variant.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/pattern/to_string.hh"
#include "cc/bzd/utility/source_location.hh"

namespace bzd {

/// Platform agnostic error type.
enum class ErrorType : bzd::UInt8
{
	/// Any error that do not fall in the below categories.
	failure,
	/// Happens when a timeout occurs, it could provide information to the handler
	/// that either the timeout was too short or the input was too slow.
	timeout,
	/// Happens while accessing a busy ressource for example,
	/// this type of error notifies the handler that it can be retried if necessary.
	busy,
	/// Happens when a stream reaches the end of its channel for example.
	eof,
	/// Error related to data received, it can be due to data corruption, etc. Generally,
	/// reason that are out of our control.
	data
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
	template <bzd::concepts::constexprStringView T>
	constexpr Error(const SourceLocation& location, const ErrorType type, const T) noexcept :
		source_{location.getFileName()}, line_{location.getLine()}, type_{type}, message_{inPlaceIndex<0>, T::value()}
	{
	}

	constexpr Error(const SourceLocation& location, const ErrorType type) noexcept :
		source_{location.getFileName()}, line_{location.getLine()}, type_{type}, message_{inPlaceIndex<0>, "No message"}
	{
	}

	template <class... Args>
	constexpr Error(const SourceLocation& location, const ErrorType type, Args&&... args) noexcept :
		source_{location.getFileName()}, line_{location.getLine()}, type_{type}, message_{inPlaceIndex<1>, reserveBuffer()}
	{
		auto& maybeString = message_.template get<Optional<interface::String&>>();
		if (maybeString)
		{
			bzd::toString(maybeString.valueMutable().assigner(), bzd::forward<Args>(args)...);
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
		case ErrorType::eof:
			return "eof"_sv;
		case ErrorType::data:
			return "data"_sv;
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

} // namespace bzd

namespace bzd::error {

template <class... Args>
struct Failure : public bzd::ResultError<bzd::Error>
{
	Failure(Args&&... args, const SourceLocation location = SourceLocation::current()) noexcept :
		bzd::ResultError<bzd::Error>{location, ErrorType::failure, bzd::forward<Args>(args)...}
	{
	}
};

template <class... Args>
Failure(Args&&...) -> Failure<Args...>;

template <class... Args>
struct Timeout : public bzd::ResultError<bzd::Error>
{
	Timeout(Args&&... args, const SourceLocation location = SourceLocation::current()) noexcept :
		bzd::ResultError<bzd::Error>{location, ErrorType::timeout, bzd::forward<Args>(args)...}
	{
	}
};

template <class... Args>
Timeout(Args&&...) -> Timeout<Args...>;

template <class... Args>
struct Busy : public bzd::ResultError<bzd::Error>
{
	Busy(Args&&... args, const SourceLocation location = SourceLocation::current()) noexcept :
		bzd::ResultError<bzd::Error>{location, ErrorType::busy, bzd::forward<Args>(args)...}
	{
	}
};

template <class... Args>
Busy(Args&&...) -> Busy<Args...>;

template <class... Args>
struct Eof : public bzd::ResultError<bzd::Error>
{
	Eof(Args&&... args, const SourceLocation location = SourceLocation::current()) noexcept :
		bzd::ResultError<bzd::Error>{location, ErrorType::eof, bzd::forward<Args>(args)...}
	{
	}
};

template <class... Args>
Eof(Args&&...) -> Eof<Args...>;

template <class... Args>
struct Data : public bzd::ResultError<bzd::Error>
{
	Data(Args&&... args, const SourceLocation location = SourceLocation::current()) noexcept :
		bzd::ResultError<bzd::Error>{location, ErrorType::data, bzd::forward<Args>(args)...}
	{
	}
};

template <class... Args>
Data(Args&&...) -> Data<Args...>;

} // namespace bzd::error
