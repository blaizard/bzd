#pragma once

#include "cc/bzd/container/impl/span.hh"
#include "cc/bzd/type_traits/is_convertible.hh"
#include "cc/bzd/type_traits/is_trivially_copyable.hh"

#include <new> // Required for placement new.

namespace bzd::impl {

/// Implementation of a contiguous storage container.
template <class T, class Storage>
class SpanResizeable : public Span<T, Storage>
{
protected:
    using Parent = Span<T, Storage>;

public: // Constructor/assignment
	// Default/copy/move constructor/assignment.
	SpanResizeable() = default;
    SpanResizeable(const SpanResizeable&) = default;
	SpanResizeable& operator=(const SpanResizeable&) = default;
	SpanResizeable(SpanResizeable&&) = default;
	SpanResizeable& operator=(SpanResizeable&&) = default;

	constexpr SpanResizeable(const Storage& storage, const bzd::Size capacity) noexcept : Parent{storage}, capacity_{capacity} {}

public: // Size.
	/// \brief Returns the maximum number of elements the container can hold.
	///
	/// \return Maximum number of element this container can hold.
	[[nodiscard]] constexpr Size capacity() const noexcept { return this->capacity_; }
	[[nodiscard]] constexpr Bool full() const noexcept { return this->capacity_ == this->size(); }

public: // Modifiers.
	/// \brief Change the size of the container.
	///
	/// \param n The new size. Note, it must a be lower or equal to the capacity.
	constexpr void resize(const Size n) noexcept
	{
		const auto newSize = (n < capacity_) ? n : capacity_;
		// Destroy elements that will potentially be removed.
		for (Size index{newSize}; index < this->storage_.size(); ++index)
		{
			this->at(index).~T();
		}
		// Initialize elements that will be potentially added.
		for (Size index{this->storage_.size()}; index < newSize; ++index)
		{
			::new (&(this->at(index))) T{};
		}
		this->storage_.sizeMutable() = newSize;
	}

	/// \brief Removes all elements from the container.
	constexpr void clear() noexcept
    {
        resize(0);
    }

	/// \brief Adds a new element at the end of the container, after its current last
	/// element.
	/// The content of `value` is copied (or moved) to the new element.
	///
	/// \param value Value to be copied (or moved) to the new element.
	template <class U>
	requires concepts::convertible<U, T>
	constexpr Size pushBack(U&& value) noexcept 
	{
		if (full())
		{
			return 0u;
		}
		this->at(this->size()) = bzd::forward<U>(value);
		++this->storage_.sizeMutable();
		return 1u;
	}

	constexpr Size pushBack(const Byte value) noexcept requires (sizeof(T) == 1u && typeTraits::isTriviallyCopyable<T>)
	{
		return pushBack(reinterpret_cast<const T&>(value));
	}

	template <class Iterator>
	requires concepts::forwardIterator<Iterator>
	constexpr Size pushBack(Iterator first, Iterator last) noexcept
	{
		Size count = 0u;
		while (!(first == last))
		{
			count += pushBack(*first);
			++first;
		}
		return count;
	}

	template <class Range>
	requires concepts::forwardRange<Range>
	constexpr Size pushBack(Range&& range) { return pushBack(bzd::begin(range), bzd::end(range)); }

	/// Appends a new element to the end of the container.
	/// The element is constructed through using placement-new in-place.
	///
	/// \param args... Arguments forwarded to the constructor.
	template <class... Args>
	constexpr Size emplaceBack(Args&&... args) noexcept
	{
		if (full())
		{
			return 1u;
		}
		auto it = this->end();
		this->emplace(it, bzd::forward<Args>(args)...);
		++this->storage_.sizeMutable();
		return 0u;
	}

protected:
	const bzd::Size capacity_{};
};

}
