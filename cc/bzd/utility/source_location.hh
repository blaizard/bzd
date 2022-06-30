#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {

class SourceLocation
{
public:
	using LineType = UInt32;

public:
	static constexpr SourceLocation current(const char* file = __builtin_FILE(),
											const char* fileName = toFileName(__builtin_FILE()),
											const char* function = __builtin_FUNCTION(),
											LineType line = __builtin_LINE()) noexcept
	{
		return SourceLocation{file, fileName, function, line};
	}

	constexpr LineType getLine() const noexcept { return line_; }
	constexpr const char* getFile() const noexcept { return file_; }
	constexpr const char* getFileName() const noexcept { return fileName_; }
	constexpr const char* getFunction() const noexcept { return function_; }

private:
	constexpr SourceLocation(const char* file, const char* fileName, const char* function, LineType line) noexcept :
		file_{file}, fileName_{fileName}, function_{function}, line_{line}
	{
	}

	static constexpr const char* toFileName(const char* file) noexcept
	{
		bzd::Int32 last = -1;
		for (bzd::Int32 index = 0; file[index] != '\0'; ++index)
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
	LineType line_;
};
} // namespace bzd
