# Components

This framework offers component based development. Components comes at various level of the development and can be associated to become a larger component. Components can also contains sub-components. It defines a composable entity which roles can vary from a driver to a functional component.

A component contains 2 entities, an interface, that is shared between all components of the same type and an implementation specific configuration.

For exampe, we will define a square which interface is a generic shape.

```bdl
interface Shape
{
	using Area = Integer [min(0)];
	method surface() -> Area;
}

component Square : Shape
{
config:
	width = Integer;
}
```

A square is defined by its border width. Such component is instantiated from a composable scope:

```bdl
composition
{
	square = Square(width = 12);
}
```

This component exposes the `Shape` interface.

## Sub-components

Components can be made from other components. The syntax is similar to a top level component, except that everything takes part in the component definition.

```bdl
component ComplexShape : Shape
{
config:
	width1 = Integer;
	width2 = Integer;

composition:
	square1 = Square(width = width1);
	square2 = Square(width = width2);
}
```

The C++ implementation of the complex component could be something like:

```c++
Area ComplexShape::surface()
{
	return square1.surface() + square2.surface();
}
```

Sub-components helps for encapsulation and hence reduce the complexity and composability while improving re-usability.
For example, imagine a complex sub-system:

```bdl
component RobotLeg : Kinematics
{
config:
	actuator1 = Actuator;
	actuator2 = Actuator;
	actuator3 = Actuator;
	logger = Logger;

composition:
	connect(this.actuator1.error, logger);
	connect(this.actuator2.error, logger);
	connect(this.actuator3.error, logger);
}
```
