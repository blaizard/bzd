#pragma once

#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/ranges/view_interface.hh"
#include "cc/bzd/utility/ranges/views/adaptor.hh"

namespace bzd::ranges {

template <class Range>
class Ref : public ViewInterface<Ref<Range>>
{
public:
	constexpr explicit Ref(Range&& range) noexcept
	requires(!concepts::view<Range>)
		: range_{range}
	{
	}

public:
	constexpr auto begin() const noexcept { return bzd::begin(range_.get()); }
	constexpr auto end() const noexcept { return bzd::end(range_.get()); }

private:
	bzd::ReferenceWrapper<Range> range_;
};

inline constexpr Adaptor<Ref> ref;

} // namespace bzd::ranges

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::ranges::Ref<Range>> = true;
}
