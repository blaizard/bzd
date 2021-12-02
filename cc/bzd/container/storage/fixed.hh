#pragma once

#include "cc/bzd/algorithm/copy_n.hh"
#include "cc/bzd/container/iterator/distance.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/min.hh"

namespace bzd::impl {
/// \brief Fixed storage type.
template <class T, bzd::SizeType N>
class FixedStorage
{
public:
	using Self = FixedStorage<T, N>;
	using DataType = const T;
	using DataMutableType = T;

public: // Constructors
	// Default/copy/move constructor/assignment.
	constexpr FixedStorage() noexcept = default;
	constexpr FixedStorage(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr FixedStorage(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	template <class... Args>
	constexpr explicit FixedStorage(InPlace, Args&&... args) noexcept : data_{bzd::forward<Args>(args)...}
	{
	}

	constexpr FixedStorage(const T* begin, const T* end) noexcept
	{
		const auto n = bzd::min(N, static_cast<SizeType>(end - begin));
		bzd::algorithm::copyN(begin, n, data_);
	}

public: // Accessors
	constexpr DataType* data() const noexcept { return data_; }
	constexpr DataMutableType* dataMutable() noexcept { return data_; }
	constexpr bzd::SizeType size() const noexcept { return N; }

private:
	T data_[(N == 0) ? 1 : N]{};
};
} // namespace bzd::impl
