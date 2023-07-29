#pragma once

#include "cc/bzd/container/reference_wrapper.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/iterator.hh"

namespace bzd::iterator {

/// Used to configure the InputOrOutputReference iterator.
/// Note, a struct is used in order to ease some function in the type_traits library,
/// namely is_same_template where arguments must be a type to work.
template <typeTraits::IteratorCategory iteratorCategory>
struct InputOrOutputReferencePolicies
{
	static constexpr auto category = iteratorCategory;
};

/// Reference to an input and/or output iterator.
///
/// This is used to implement single pass range for example.
template <concepts::inputOrOutputIterator Iterator, class Policies>
class InputOrOutputReference : public typeTraits::IteratorBase
{
public: // Traits
	using Self = InputOrOutputReference;
	using DifferenceType = typename typeTraits::IteratorDifference<Iterator>;
	using ValueType = typename typeTraits::IteratorValue<Iterator>;
	static constexpr auto category = Policies::category;

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
		++(it_.get());
		return *this;
	}

	[[nodiscard]] constexpr ValueType& operator*() const noexcept { return *(it_.get()); }
	[[nodiscard]] constexpr ValueType* operator->() const noexcept { return &(*(it_.get())); }

private:
	friend constexpr Bool operator==(const InputOrOutputReference<Iterator, Policies>& it1, const Iterator& it2) noexcept
	{
		return it1.it_.get() == it2;
	}
	friend constexpr Bool operator==(const Iterator& it1, const InputOrOutputReference<Iterator, Policies>& it2) noexcept
	{
		return it2.it_.get() == it1;
	}
	friend constexpr Bool operator!=(const InputOrOutputReference<Iterator, Policies>& it1, const Iterator& it2) noexcept
	{
		return it1.it_.get() != it2;
	}
	friend constexpr Bool operator!=(const Iterator& it1, const InputOrOutputReference<Iterator, Policies>& it2) noexcept
	{
		return it2.it_.get() != it1;
	}

	bzd::ReferenceWrapper<Iterator> it_;
};

} // namespace bzd::iterator
