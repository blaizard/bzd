#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::test {

/// Create custome range of various types.
template <class CategoryTag, class T = bzd::Int32, Size capacity = 42u>
class Range
{
private:
	using Container = bzd::Array<T, capacity>;

public:
	class Iterator : public Container::Iterator
	{
	public: // Traits
		using Category = CategoryTag;
	};

	class ConstIterator : public Container::ConstIterator
	{
	public: // Traits
		using Category = CategoryTag;
	};

public: // Constructor.
	template <class... Args>
	explicit constexpr Range(Args&&... args) noexcept : data_{bzd::forward<Args>(args)...}
	{
	}

public: // Iterators
	[[nodiscard]] constexpr auto begin() noexcept { return Iterator{data_.begin()}; }
	[[nodiscard]] constexpr auto begin() const noexcept { return ConstIterator{data_.begin()}; }
	[[nodiscard]] constexpr auto end() noexcept { return Iterator{data_.end()}; }
	[[nodiscard]] constexpr auto end() const noexcept { return ConstIterator{data_.end()}; }

private:
	Container data_;
};

} // namespace bzd::test
