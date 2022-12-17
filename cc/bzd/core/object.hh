#pragma once

namespace bzd {

template <class T>
class ValueGetter
{
public:
	constexpr ValueGetter() noexcept {}

public: // API.
	constexpr const T& last() const noexcept { return T{}; }

private:
};

} // namespace bzd
