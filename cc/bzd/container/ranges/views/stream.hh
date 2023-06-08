#pragma once

#include "cc/bzd/container/iterator/input_or_output_reference.hh"
#include "cc/bzd/container/ranges/view_interface.hh"
#include "cc/bzd/container/ranges/views/adaptor.hh"
#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/distance.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::ranges {

/// A stream is a range that is single pass. It must be of type input or output,
/// and is fully compatible with its range counter part.
///
/// Its begin iterator is persistent, meaning that when an increment is performed,
/// it remains incremented even after subsequent call of begin().
///
/// It may be used for streaming data from or into, therefore is a great candidate
/// for things like serialization, format...
/// Stream may or may not be bounded, in the latter case their sentinel will never
/// be reached and their size will be infinite.
template <concepts::borrowedRange Range>
class Stream : public ViewInterface<Stream<Range>>
{
private: // Traits.
	using Iterator = typeTraits::RangeIterator<Range>;

public:
	constexpr Stream(bzd::InPlace, auto&& range) noexcept : range_{bzd::forward<decltype(range)>(range)}, it_{bzd::begin(range_.get())} {}

	Stream(const Stream&) = delete;
	Stream& operator=(const Stream&) = delete;
	constexpr Stream(Stream&& other) noexcept : range_{bzd::move(other.range_)}, it_{bzd::begin(range_.get())}
	{
		if constexpr (concepts::forwardRange<Range>)
		{
			// Note this is UB as the range that is moved is used afterward.
			// To be changed.
			const auto n = bzd::distance(bzd::begin(other.range_.get()), other.it_);
			bzd::advance(it_, n);
		}
	}
	constexpr Stream& operator=(Stream&& other) noexcept
	{
		[[maybe_unused]] const auto n = bzd::distance(bzd::begin(other.range_.get()), other.it_);
		range_ = bzd::move(other.range_);
		it_ = bzd::begin(range_.get());
		if constexpr (concepts::forwardRange<Range>)
		{
			bzd::advance(it_, n);
		}
		return *this;
	}
	~Stream() = default;

public:
	constexpr auto begin() noexcept
	{
		if constexpr (concepts::sameTemplate<Iterator, iterator::InputOrOutputReference>)
		{
			return it_;
		}
		else
		{
			constexpr auto category =
				typeTraits::iteratorCategory<Iterator> & (typeTraits::IteratorCategory::input | typeTraits::IteratorCategory::output);
			return iterator::InputOrOutputReference<Iterator, iterator::InputOrOutputReferencePolicies<category>>{it_};
		}
	}
	constexpr auto end() const noexcept { return bzd::end(range_.get()); }

	/// A Stream cannot have a size eventhough, it could have if it comes from a sized range.
	/// Deleting it ensure no wrong uses that would make it incompatible with the concept.
	constexpr auto size() const noexcept = delete;

protected:
	bzd::Wrapper<Range> range_;
	Iterator it_;
};

template <class Range>
Stream(bzd::InPlace, Range&&) -> Stream<Range&&>;

inline constexpr Adaptor<Stream> stream;

} // namespace bzd::ranges

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::ranges::Stream<Range>> = concepts::borrowedRange<Range>;
}
