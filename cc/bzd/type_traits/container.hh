#pragma once

#include "cc/bzd/container/span.hh"
#include "cc/bzd/type_traits/is_convertible.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::concepts {

/// Defines a container that can be expanded by adding new data.
template <class Container, class Value>
concept appendable = requires(Container container, Value value) {
						 {
							 container.append(value)
							 } -> bzd::concepts::convertible<bzd::Size>;
					 };

template <class Container>
concept appendableWithBytes = appendable<Container, bzd::Span<const bzd::Byte>>;

template <class Container, class Value>
concept reducible = requires(Container container, Value& value) {
						{
							container.reduce(value)
							} -> bzd::concepts::convertible<Value&>;
					};

} // namespace bzd::concepts
