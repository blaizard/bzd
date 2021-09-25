#pragma once

namespace bzd {
class Panic
{
public:
	typedef void (*Type)();

	static void trigger();
	static void setDefault(Type panic) noexcept;
};
} // namespace bzd
