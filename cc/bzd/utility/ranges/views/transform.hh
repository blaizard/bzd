#pragma once

#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/iterators/advance.hh"
#include "cc/bzd/utility/ranges/view_interface.hh"
#include "cc/bzd/utility/ranges/views/adaptor.hh"
#include "cc/bzd/utility/ranges/views/adaptor_iterator.hh"

namespace bzd::ranges {

template <concepts::borrowedRange Range, class Function>
class Transform : public ViewInterface<Transform<Range, Function>>
{
private: // Traits.
	class Iterator : public AdapatorForwardIterator<Range, Iterator>
	{
	public:
		constexpr Iterator(auto&& it, const Function& func) noexcept : AdapatorForwardIterator<Range, Iterator>{bzd::move(it)}, func_{func}
		{
		}
		constexpr auto get(const auto& it) const noexcept { return func_.get()(*it); }

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
	bzd::Wrapper<Range> range_;
	Function func_;
};

template <class Range, class Function>
Transform(bzd::InPlace, Range&&, Function) -> Transform<Range&&, typeTraits::RemoveReference<Function>>;

inline constexpr Adaptor<Transform> transform;

} // namespace bzd::ranges
