#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/view_storage.hh"
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
		constexpr Iterator(IteratorBase&& it, const Function& func) noexcept : IteratorBase{bzd::move(it)}, func_{func} {}

		constexpr auto operator*() const noexcept { return func_.get()(IteratorBase::operator*()); }
		// TODO: add overloads for all accessors

	private:
		bzd::ReferenceWrapper<const Function> func_;
	};

public:
	constexpr Transform(bzd::InPlace, auto&& range, const Function& func) noexcept :
		range_{bzd::forward<decltype(range)>(range)}, func_{func}
	{
	}

public:
	constexpr auto begin() const noexcept { return Iterator{bzd::begin(range_.get()), func_}; }
	constexpr auto end() const noexcept { return bzd::end(range_.get()); }

private:
	ViewStorage<Range> range_;
	Function func_;
};

template <class Range, class Function>
Transform(bzd::InPlace, Range&&, Function) -> Transform<Range&&, typeTraits::RemoveReference<Function>>;

inline constexpr Adaptor<Transform> transform;

} // namespace bzd::range
