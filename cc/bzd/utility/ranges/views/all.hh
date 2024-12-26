#pragma once

#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/is_lvalue_reference.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/ranges/view_interface.hh"
#include "cc/bzd/utility/ranges/views/adaptor.hh"
#include "cc/bzd/utility/ranges/views/owning.hh"
#include "cc/bzd/utility/ranges/views/ref.hh"

namespace bzd::ranges {

template <class Range>
using All = bzd::typeTraits::Conditional<concepts::view<Range>,
										 typeTraits::RemoveReference<Range>,
										 bzd::typeTraits::Conditional<typeTraits::isLValueReference<Range>,
																	  Ref<typeTraits::RemoveReference<Range>>,
																	  Owning<typeTraits::RemoveReference<Range>>>>;

// A template specialization is needed because we cannot have deduction guides with template aliases (using).
template <>
struct Adaptor<All>
{
	[[nodiscard]] constexpr auto operator()() const noexcept
	{
		return [](auto&& range) { return All<decltype(range)>(bzd::move(range)); };
	}
};

inline constexpr Adaptor<All> all;

} // namespace bzd::ranges
