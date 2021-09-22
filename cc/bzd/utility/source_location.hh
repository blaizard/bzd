#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {

class SourceLocation
{
public:
	static constexpr SourceLocation current(const char* file = __builtin_FILE(),
											const char* fileName = toFileName(__builtin_FILE()),
											const char* function = __builtin_FUNCTION(),
											UInt32Type line = __builtin_LINE()) noexcept
	{
		return SourceLocation{file, fileName, function, line};
	}

	constexpr UInt32Type getLine() const noexcept { return line_; }
	constexpr const char* getFile() const noexcept { return file_; }
	constexpr const char* getFileName() const noexcept { return fileName_; }
	constexpr const char* getFunction() const noexcept { return function_; }

private:
	constexpr SourceLocation(const char* file, const char* fileName, const char* function, UInt32Type line) :
		file_{file}, fileName_{fileName}, function_{function}, line_{line}
	{
	}

	static constexpr const char* toFileName(const char* file) noexcept
	{
		bzd::Int32Type last = -1;
		for (bzd::Int32Type index = 0; file[index] != '\0'; ++index)
		{
			if (file[index] == '/')
			{
				last = index;
			}
		}

		if (last == -1)
		{
			return file;
		}
		return &file[last + 1];
	}

	const char* const file_;
	const char* const fileName_;
	const char* const function_;
	UInt32Type line_;
};
} // namespace bzd
