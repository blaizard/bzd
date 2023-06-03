#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::range {

template <concepts::borrowedRange Range, class Function>
class Transform : public ViewInterface<Transform<Range, Function>>
{
private: // Traits.
	using IteratorBase = typeTraits::RangeIterator<Range>;
	class Iterator : public IteratorBase
	{
	public:
		constexpr Iterator(IteratorBase&& it, Function& func) noexcept : IteratorBase{bzd::move(it)}, func_{func} {}

		constexpr auto operator*() const noexcept { return func_(IteratorBase::operator*()); }
		// TODO: add overloads for all accessors

	private:
		Function& func_;
	};

public:
	constexpr Transform(bzd::InPlace, auto&& range, auto&& func) noexcept :
		range_{bzd::forward<decltype(range)>(range)}, func_{bzd::forward<decltype(func)>(func)}
	{
	}

public:
	constexpr auto begin() const noexcept { return Iterator{bzd::begin(range_), func_}; }
	constexpr auto end() const noexcept { return bzd::end(range_); }

private:
	Range range_;
	Function func_;
};

template <class Range, class Function>
Transform(bzd::InPlace, Range&&, Function&&) -> Transform<Range&&, Function&&>;

inline constexpr Adaptor<Transform> transform;

} // namespace bzd::range
