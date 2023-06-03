#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/remove_reference.hh"

namespace bzd {
template <class T>
class ValueWrapper
{
private:
	using Self = ValueWrapper<T>;
	using Value = bzd::typeTraits::RemoveReference<T>;

public: // Constructor.
	constexpr explicit ValueWrapper(const Value& value) noexcept : value_{value} {}
	ValueWrapper(const ValueWrapper&) = default;
	ValueWrapper& operator=(const ValueWrapper&) = default;
	ValueWrapper(ValueWrapper&&) = default;
	ValueWrapper& operator=(ValueWrapper&&) = default;
	~ValueWrapper() = default;

public: // API.
	[[nodiscard]] constexpr operator Value&() const noexcept { return value_; }
	[[nodiscard]] constexpr Value& get() noexcept { return value_; }
	[[nodiscard]] constexpr const Value& get() const noexcept { return value_; }
	[[nodiscard]] constexpr Value* operator->() noexcept { return &value_; }
	[[nodiscard]] constexpr const Value* operator->() const noexcept { return &value_; }

public: // Comparison operators.
	[[nodiscard]] constexpr bzd::Bool operator==(const Self& other) const noexcept { return (value_ == other.value_); }
	[[nodiscard]] constexpr bzd::Bool operator!=(const Self& other) const noexcept { return !(*this == other); }

private:
	Value value_;
};

} // namespace bzd
