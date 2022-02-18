#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/iterator.hh"

namespace bzd::iterator {

template <class Iterator>
class ContainerOfIterables : public typeTraits::IteratorBase
{
public: // Traits.
	using Self = ContainerOfIterables<Iterator>;
	using Category = typeTraits::ForwardTag;
	using ContainerValueType = typename Iterator::ValueType;
	using NestedIterator = typename ContainerValueType::Iterator;
	using IndexType = typename NestedIterator::IndexType;
	using DifferenceType = typename NestedIterator::DifferenceType;
	using ValueType = typename NestedIterator::ValueType;

public: // Constructors.
	constexpr ContainerOfIterables(Iterator it, Iterator end) noexcept : it_{it}, end_{end}, nested_{} { ++(*this); }
	constexpr explicit ContainerOfIterables(Iterator end) noexcept : it_{end}, end_{end}, nested_{} {}

	constexpr ContainerOfIterables(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr ContainerOfIterables(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

public: // Modifiers.
	constexpr Self& operator++() noexcept
	{
		do
		{
			if (nested_)
			{
				++(nested_.valueMutable());
			}
			else
			{
				nested_ = it_->begin();
			}

			if (nested_.value() != it_->end())
			{
				break;
			}
			++it_;
			nested_.reset();
		} while (it_ != end_);

		return *this;
	}

	constexpr Self operator++(int) noexcept
	{
		Self it{*this};
		++(*this);
		return it;
	}

public: // Comparators.
	[[nodiscard]] constexpr bool operator==(const Self& other) const noexcept { return (it_ == other.it_) && (nested_ == other.nested_); }
	[[nodiscard]] constexpr bool operator!=(const Self& other) const noexcept { return !(other == *this); }

public: // Accessors.
	[[nodiscard]] constexpr ValueType& operator*() const { return *(nested_.value()); }
	[[nodiscard]] constexpr ValueType* operator->() const { return &(*(nested_.value())); }

private:
	Iterator it_;
	Iterator end_;
	bzd::Optional<NestedIterator> nested_;
};

} // namespace bzd::iterator
