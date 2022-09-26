#pragma once

#include "cc/bzd/container/span.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::concepts {

/// Defines a container that can be expanded by adding new data.
template <class Container, class Value>
concept containerAppendable = requires(Container container, Value value)
{
	container.append(value);
};

template <class Container>
concept containerToString = requires(Container container)
{
	container.append(bzd::Span<const bzd::Byte>{});
};

template <class Container>
concept containerFromString = outputRange<Container>;

} // namespace bzd::concepts
