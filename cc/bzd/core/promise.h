#pragma once

namespace bzd::interface {
class Promise
{
protected:
	constexpr Promise() noexcept = default;
};
} // namespace bzd::interface

namespace bzd {
class Promise : bzd::interface::Promise
{
public:
	constexpr Promise() noexcept : interface::Promise{} {}
};
} // namespace bzd
