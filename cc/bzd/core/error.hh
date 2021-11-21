#pragma once

namespace bzd {
enum class ErrorType
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
/// the error handler could react.
/// - The error location to trace the error for debugging purpose.
/// - A human readable message that describes details of the actual
/// error, which could include for example, implementation specific information.
class Error
{
public:
	Error();

private:
	ErrorType type_;
	StringView source_;
	SizeType line_;
	String<64> message_;
};

void error() {}
} // namespace bzd

/// Helper to propagate an error on an asynchronous call.
/// After this call, it asserted that the result contains a valid result.
#define ASSERT_ASYNC_RESULT(result)    \
	if (!(result))                     \
	{                                  \
		co_return(result).propagate(); \
	}
