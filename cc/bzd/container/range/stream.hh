#pragma once

#include "cc/bzd/container/iterator/input_or_output_reference.hh"
#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/distance.hh"

namespace bzd::range {

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
template <concepts::inputOrOutputIterator Iterator, concepts::sentinelFor<Iterator> Sentinel>
class Stream : public ViewInterface<Stream<Iterator, Sentinel>>
{
public:
	constexpr Stream(Iterator begin, Sentinel end) noexcept : it_{begin}, end_{end} {}
	template <concepts::inputOrOutputRange Range>
	constexpr explicit Stream(Range&& range) noexcept : it_{bzd::begin(range)}, end_{bzd::end(range)}
	{
	}

	Stream(const Stream&) = delete;
	Stream& operator=(const Stream&) = delete;
	Stream(Stream&&) = delete;
	Stream& operator=(Stream&&) = delete;
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
			using Policies = iterator::InputOrOutputReferencePolicies<category | typeTraits::IteratorCategory::stream>;
			return iterator::InputOrOutputReference<Iterator, Policies>{it_};
		}
	}
	constexpr auto end() const noexcept { return end_; }

	/// A Stream cannot have a size eventhough, it could have if it comes from a sized range.
	/// Deleting it ensure no wrong uses that would make it incompatible with the concept.
	constexpr auto size() const noexcept = delete;

protected:
	Iterator it_;
	Sentinel end_;
};

template <concepts::inputOrOutputRange Range>
Stream(Range&& range) -> Stream<typeTraits::RangeIterator<Range>, typeTraits::RangeSentinel<Range>>;

} // namespace bzd::range
