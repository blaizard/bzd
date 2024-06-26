#pragma once

namespace bzd::ranges {

/// Results used to associate an input with its output.
/// This is mainly used by the algorithm library.
template <class In, class Out>
struct InOutResult
{
	constexpr InOutResult(In& i, Out& o) noexcept : in{i}, out{o} {}
	In in;
	Out out;
};

} // namespace bzd::ranges
