#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::range {

template <concepts::borrowedRange V, class F>
class Transform : public ViewInterface<Transform<V, F>>
{
private: // Traits.
	using IteratorBase = typeTraits::RangeIterator<V>;
	using Sentinel = typeTraits::RangeSentinel<V>;
	class Iterator : public IteratorBase
	{
	public:
		constexpr Iterator(IteratorBase&& it, F&& func) noexcept : IteratorBase{bzd::move(it)}, func_{bzd::move(func)} {}

		constexpr auto operator*() const noexcept { return func_(IteratorBase::operator*()); }
		// TODO: add overloads for all accessors

	private:
		F func_;
	};

public:
	constexpr Transform(bzd::InPlace, auto&& view, auto&& func) noexcept : begin_{bzd::begin(view), bzd::move(func)}, end_{bzd::end(view)}
	{
	}

public:
	constexpr Iterator begin() const noexcept { return begin_; }
	constexpr Sentinel end() const noexcept { return end_; }

private:
	Iterator begin_;
	Sentinel end_;
};

template <class T, class F>
Transform(bzd::InPlace, T&&, F&&) -> Transform<T&&, typeTraits::RemoveReference<F>>;

inline constexpr Adaptor<Transform> transform;

} // namespace bzd::range
