#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd::range {

/// An Owning object is a view that has unique ownership of a range. It is move-only and stores that range within it.
template <concepts::range Range>
class Owning : public ViewInterface<Owning<Range>>
{
public:
	constexpr Owning(bzd::InPlace, Range&& range) noexcept : range_{bzd::move(range)} {}
	Owning(const Owning&) = delete;
	Owning& operator=(const Owning&) = delete;
	Owning(Owning&&) = default;
	Owning& operator=(Owning&&) = default;
	~Owning() = default;

public:
	constexpr auto begin() noexcept { return range_.begin(); }
	constexpr auto begin() const noexcept { return range_.begin(); }
	constexpr auto end() noexcept { return range_.end(); }
	constexpr auto end() const noexcept { return range_.end(); }
	constexpr auto size() const noexcept
	requires(concepts::sizedRange<Range>)
	{
		return bzd::size(range_);
	}

private:
	Range range_;
};

inline constexpr Adaptor<Owning> owning;

} // namespace bzd::range

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::range::Owning<Range>> = concepts::borrowedRange<Range>;
}
