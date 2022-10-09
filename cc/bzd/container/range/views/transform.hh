#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::range {

template <concepts::range V, class F>
class Transform : public ViewInterface<Transform<V, F>>
{
public: // Traits.
	using IteratorBase = typename typeTraits::Range<V>::Iterator;
	using Sentinel = typename typeTraits::Range<V>::Sentinel;
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
	/// Note the reference is important here, otherwise the range is copied.
	/// This can therefore works for every type of ranges: owning or borrowing.
	constexpr Transform(V& view, F&& func) noexcept : begin_{bzd::begin(view), bzd::move(func)}, end_{bzd::end(view)} {}

public:
	constexpr Iterator begin() const noexcept { return begin_; }
	constexpr Sentinel end() const noexcept { return end_; }

private:
	Iterator begin_;
	Sentinel end_;
};

inline constexpr Adaptor<Transform> transform;

} // namespace bzd::range
