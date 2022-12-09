#pragma once

namespace bzd::range {

/// Results used to associate an intput with its output.
/// This is mainly used by the algorithm library.
template <class In, class Out>
struct InOutResult
{
	constexpr InOutResult(In& i, Out& o) noexcept : in{i}, out{o} {}
	In in;
	Out out;
};

} // namespace bzd::range
