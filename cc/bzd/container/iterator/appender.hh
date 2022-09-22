#pragma once

#include "cc/bzd/type_traits/iterator.hh"

namespace bzd::iterator {

/// An appender iterator is an iterator that appends new
/// data at the end of a container.
template <class Container>
class Appender : public typeTraits::IteratorBase
{
public: // Traits
	using Self = Appender;
	using Category = typeTraits::OutputTag;
	using ValueType = typename Container::ValueType;

public:
	explicit Appender(Container& container) noexcept : container_{container} {}

public: // API
	constexpr Self& operator++() noexcept { return *this; } 
	constexpr Self operator++(int) noexcept
	{
		return *this;
	}

	[[nodiscard]] constexpr Self& operator*() noexcept { return *this; }
	[[nodiscard]] constexpr Self* operator->() noexcept { return this; }

    Self& operator=(const ValueType& value) noexcept
    {
        container_.append(value);
        return *this;
    }

    Self& operator=(ValueType&& value) noexcept
    {
        container_.append(bzd::move(value));
        return *this;
    }

private:
	Container& container_;
};

} // namespace bzd::iterator
