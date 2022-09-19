#pragma once

namespace bzd::concepts {

/// Defines a container that can be expanded by adding new data.
template <class Container, class Value>
concept containerAppendable = requires(Container container, Value value)
{
	container.pushBack(value);
};

} // namespace bzd::concepts
