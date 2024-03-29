#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/remove_reference.hh"

namespace bzd {
template <class T>
class ReferenceWrapper
{
private:
	using Self = ReferenceWrapper<T>;
	using Value = bzd::typeTraits::RemoveReference<T>;

public: // Constructor.
	constexpr ReferenceWrapper(Value& value) noexcept : value_{&value} {}
	ReferenceWrapper(const ReferenceWrapper&) = default;
	ReferenceWrapper& operator=(const ReferenceWrapper&) = default;
	ReferenceWrapper(ReferenceWrapper&&) = default;
	ReferenceWrapper& operator=(ReferenceWrapper&&) = default;
	~ReferenceWrapper() = default;

public: // API.
	[[nodiscard]] constexpr operator Value&() const noexcept { return *value_; }
	[[nodiscard]] constexpr Value& get() const noexcept { return *value_; }
	[[nodiscard]] constexpr Value* operator->() const noexcept { return &(*value_); }

public: // Comparison operators.
	[[nodiscard]] constexpr bzd::Bool operator==(const Self& other) const noexcept { return (*value_ == *(other.value_)); }
	[[nodiscard]] constexpr bzd::Bool operator!=(const Self& other) const noexcept { return !(*this == other); }

private:
	Value* value_;
};

template <class T>
[[nodiscard]] constexpr auto ref(T& value) noexcept
{
	return ReferenceWrapper<T>{value};
}

template <class T>
[[nodiscard]] constexpr auto cref(const T& value) noexcept
{
	return ReferenceWrapper<const T>{value};
}

} // namespace bzd
