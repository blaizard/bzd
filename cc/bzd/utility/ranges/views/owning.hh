#pragma once

#include "cc/bzd/utility/move.hh"
#include "cc/bzd/utility/ranges/view_interface.hh"
#include "cc/bzd/utility/ranges/views/adaptor.hh"

namespace bzd::ranges {

/// An Owning object is a view that has unique ownership of a range. It is move-only and stores that range within it.
template <class Range>
class Owning : public ViewInterface<Owning<Range>>
{
public:
	constexpr Owning(Range&& range) noexcept
	requires(!concepts::view<Range>)
		: range_{bzd::move(range)}
	{
	}
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

private:
	Range range_;
};

inline constexpr Adaptor<Owning> owning;

} // namespace bzd::ranges

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::ranges::Owning<Range>> = concepts::borrowedRange<Range>;
}
