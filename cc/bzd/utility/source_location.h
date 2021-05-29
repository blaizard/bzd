#pragma once

#include "cc/bzd/platform/types.h"

namespace bzd {

class SourceLocation
{
public:
	static constexpr SourceLocation current(const char* file = __builtin_FILE(),
											const char* function = __builtin_FUNCTION(),
											UInt32Type line = __builtin_LINE()) noexcept
	{
		return SourceLocation{file, function, line};
	}

	constexpr UInt32Type getLine() const noexcept { return line_; }
	constexpr const char* getFile() const noexcept { return file_; }
	constexpr const char* getFunction() const noexcept { return function_; }

private:
	constexpr SourceLocation(const char* file, const char* function, UInt32Type line) : file_{file}, function_{function}, line_{line} {}

	const char* const file_;
	const char* const function_;
	UInt32Type line_;
};
} // namespace bzd
