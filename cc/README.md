# C++

## Directory Layout

- bdl: Bazel rules and generators for the `bdl` (Bzd Description Language) ecosystem.
- bzd: contains the standard library providing building blocks for more complex applications.
- components: contains generic and platform specific components that can be composed via the `bdl` language.
  Note that platform specific libraries can only be linked to a target but not to the application level.
- libs: 3rd party and own libraries to build complex generic functionalities.
- targets: supported target platforms, containing for example startup files and composition layout.
  This is where the device specific dependencies come from.
