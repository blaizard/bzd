#pragma once

#include "cc/bzd/meta/choose_nth.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::meta::impl {

// Class delcaration.
template <Size index, class... Ts>
class Tuple;

// Base case.
template <Size index>
class Tuple<index>
{
};

template <Size index, class T, class... Ts>
class Tuple<index, T, Ts...> : public Tuple<index + 1, Ts...>
{
public:
	using Type = T;
};

} // namespace bzd::meta::impl

namespace bzd::meta {

template <typename... Ts>
class Tuple : public impl::Tuple<0, Ts...>
{
public:
	/// Get the type given a specific index.
	template <Size index>
	using Get = bzd::meta::ChooseNth<index, Ts...>;

	/// Get the number of entry of this tuple.
	///
	/// \return The number of entry.
	static constexpr Size size() noexcept { return sizeof...(Ts); }
};

} // namespace bzd::meta
