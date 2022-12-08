#pragma once

#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/iterator.hh"

namespace bzd::iterator {

/// Reference to an input and/or output iterator.
///
/// This is used to implement single pass range for example.
template <concepts::inputOrOutputIterator Iterator, typeTraits::IteratorCategory iteratorCategory>
class InputOrOutputReference : public typeTraits::IteratorBase
{
public: // Traits
	using Self = InputOrOutputReference;
	using DifferenceType = typename typeTraits::IteratorDifference<Iterator>;
	using ValueType = typename typeTraits::IteratorValue<Iterator>;
	static constexpr auto category = iteratorCategory;

public: // Constructors.
	constexpr explicit InputOrOutputReference(Iterator& it) noexcept : it_{it} {}

	InputOrOutputReference(const InputOrOutputReference&) = default;
	InputOrOutputReference& operator=(const InputOrOutputReference&) = default;
	InputOrOutputReference(InputOrOutputReference&&) = default;
	InputOrOutputReference& operator=(InputOrOutputReference&&) = default;
	~InputOrOutputReference() = default;

public: // API.
	constexpr Self& operator++() noexcept
	{
		++it_;
		return *this;
	}

	[[nodiscard]] constexpr Bool operator==(const Self& it) const noexcept { return it_ == it; }
	[[nodiscard]] constexpr Bool operator==(const Iterator& it) const noexcept { return it_ == it; }
	[[nodiscard]] constexpr Bool operator!=(const Self& it) const noexcept { return !(it == *this); }
	[[nodiscard]] constexpr Bool operator!=(const Iterator& it) const noexcept { return !(it == *this); }

	[[nodiscard]] constexpr ValueType& operator*() const noexcept { return *it_; }
	[[nodiscard]] constexpr ValueType* operator->() const noexcept { return &(*it_); }

private:
	Iterator& it_;
};

} // namespace bzd::iterator
