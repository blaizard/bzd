# C++

## Directory Layout

- bzd: contains the standard library providing building blocks for more complex applications.
- components: contains generic and platform specific components that can be composed via the `bdl` language.
  Note that platform specific libraries can only be linked to a target but not to the application level.
- libs: 3rd party and own library to build complex generic functionalities.
- targets: Target platform supported containing for example startup files and composition layout.
  This is where the device specific dependencies come from.
