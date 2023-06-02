#pragma once

#include "cc/bzd/container/iterator/input_or_output_reference.hh"
#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/in_place.hh"

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
template <concepts::borrowedRange V>
class Stream : public ViewInterface<Stream<V>>
{
private: // Traits.
	using Iterator = typeTraits::RangeIterator<V>;
	using Sentinel = typeTraits::RangeSentinel<V>;

public:
	constexpr Stream(bzd::InPlace, auto&& view) noexcept : it_{bzd::begin(view)}, end_{bzd::end(view)} {}

	Stream(const Stream&) = delete;
	Stream& operator=(const Stream&) = delete;
	Stream(Stream&&) = default;
	Stream& operator=(Stream&&) = default;
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
	constexpr auto end() const noexcept { return end_; }

	/// A Stream cannot have a size eventhough, it could have if it comes from a sized range.
	/// Deleting it ensure no wrong uses that would make it incompatible with the concept.
	constexpr auto size() const noexcept = delete;

protected:
	Iterator it_;
	Sentinel end_;
};

template <class T>
Stream(bzd::InPlace, T&&) -> Stream<T&&>;

inline constexpr Adaptor<Stream> stream;

} // namespace bzd::range

namespace bzd::typeTraits {
template <class V>
inline constexpr bzd::Bool enableBorrowedRange<bzd::range::Stream<V>> = concepts::borrowedRange<V>;
}
