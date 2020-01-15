#pragma once

#include "bzd/type_traits/remove_reference.h"

namespace bzd {
template <class T>
class ReferenceWrapper
{
private:
	using Value = typename bzd::typeTraits::removeReference<T>::type;

public:
	constexpr ReferenceWrapper(Value& value) noexcept : value_(&value) {}

	constexpr operator Value&() const noexcept { return *value_; }

	constexpr Value& get() const noexcept { return *value_; }

private:
	Value* value_;
};
} // namespace bzd
