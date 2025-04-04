#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::test {

/// Create custom range of various types.
template <typeTraits::IteratorCategory iteratorCategory, class T = bzd::Int32, Size capacity = 42u, Bool hasConst = true>
class Range
{
private:
	using Container = bzd::Array<T, capacity>;

public:
	class Iterator : public Container::Iterator
	{
	public: // Traits
		static constexpr auto category = iteratorCategory;
	};

	class ConstIterator : public Container::ConstIterator
	{
	public: // Traits
		static constexpr auto category = iteratorCategory;
	};

public: // Constructor.
	template <class... Args>
	explicit constexpr Range(Args&&... args) noexcept : data_{bzd::forward<Args>(args)...}
	{
	}

public: // Iterators
	[[nodiscard]] constexpr auto begin() noexcept { return Iterator{data_.begin()}; }
	[[nodiscard]] constexpr auto begin() const noexcept
	requires(hasConst)
	{
		return ConstIterator{data_.begin()};
	}
	[[nodiscard]] constexpr auto end() noexcept { return Iterator{data_.end()}; }
	[[nodiscard]] constexpr auto end() const noexcept
	requires(hasConst)
	{
		return ConstIterator{data_.end()};
	}

private:
	Container data_;
};

} // namespace bzd::test
