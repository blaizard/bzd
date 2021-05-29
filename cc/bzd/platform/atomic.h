#pragma once

#include "cc/bzd/container/named_type.h"

#include <atomic>

namespace bzd {
template <class T>
class Atomic : public bzd::NamedType<std::atomic<T>, struct AtomicTag, bzd::Comparable, bzd::Incrementable, bzd::Decrementable>
{
public:
	using bzd::NamedType<std::atomic<T>, struct AtomicTag, bzd::Comparable, bzd::Incrementable, bzd::Decrementable>::NamedType;

	constexpr explicit Atomic(const T& value) noexcept { this->get() = value; }

	constexpr void store(const T& v) { this->get().store(v); }

	constexpr T load() const { return this->get().load(); }

	constexpr T operator+=(const T& value) { return this->get().fetch_add(value); }
	constexpr T operator-=(const T& value) { return this->get().fetch_sub(value); }
	constexpr T operator&=(const T& value) { return this->get().fetch_and(value); }
	constexpr T operator|=(const T& value) { return this->get().fetch_or(value); }
	constexpr T operator^=(const T& value) { return this->get().fetch_xor(value); }
	constexpr explicit operator bool() const { return static_cast<bool>(this->get()); }

	constexpr T exchange(T v) { return this->get().exchange(v); }

	constexpr bool compareExchange(T& expected, T desired) { return this->get().compare_exchange_strong(expected, desired); }
};
} // namespace bzd
