# Manifest

This documents the manifest syntax.
Manifests uses yaml as a formater and have various main sections:
- objects
- interfaces

## Objects

Object sections are used to defined globally accessible objects within the application.
Such object can be access via the registry.

Here is an extract of a manifest documenting few objects:
```yaml
objects:
    "bzd::OChannel.stdout":
        implementation: !ref bzd::io::Stdout
    "bzd::Log.default":
        params:
            - !ref bzd::OChannel.stdout
```
This snippet defines an object named `stdout` of type `bzd::OChannel`. It's underlying implementation is a channel of type `bzd::io::Stdout`.
To access such object, one can simply call the follwing that will return a reference of this object.
```cpp
bzd::Registry<bzd::OChannel>::get("stdout");
```
The second object created is a logger of type `bzd::Log` with the name `default`. Loggers takes as argument objects of type `bzd::OChannel`, this
is done by referencing the previously created object. Note the syntax uses `!ref` to reference an existing object. See [Refences](#References) for more details about this keyword.

## References

Objects or any known C++ language keyword, is referenced within the manifest by the keyword `!ref`.