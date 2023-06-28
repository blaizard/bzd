# Pattern

The pattern library is a flexible framework to deal with pattern and specializations at compile time.

It allow the build up of tools like `std::format`.

## What does it do?

It takes as input and compile time string, split it into strings and specialization, delimited by `{...}`
extract metadatas for the specialization and call the right specialization function with its corresponding
argument.
A lot of sanity checks are also performed to ensure the string is properly formed and that the number
of arguments are correct.

Example, given the string "Hello {:[a-z]+}" and the argument `name` of type `bzd::String&`.
It will decompose the string into "Hello " and call the specialization of bzd::String& for this implementation
with the metadata "[a-z]+".
Then it's up to the implementation to deal with the data. All this is done during compilation time.

## Specialization

The use of a class here is to allow partial specialization.
Also function overloading cannot be used here because the definition order will matter,
as those can be used in a recursive way. This would lead to error such as:
"specialization of [...] after instanciation".
Some article on the matter: http://www.gotw.ca/publications/mill17.htm

### How to use it?

Given `MySpecialization`, a struct specialized.

```c++
template <concepts::??? T>
struct MySpecialization<T>
{
    template <concepts::inputByteCopyableRange Range>
    static constexpr Optional<Size> fromString(Range&& range, T& value) noexcept { ... }
};
```

or with metadata. In that case, the Metadata type defines the type used and the function must have a "parse" function
that parses the options string and generate a Metadata structure. This function is called during build time only, so
it must be constexpr. Metadata are optional and are parsed from the options as a StringView following the pattern: `{:<options>}`.

```c++
template <concepts::??? T>
struct MySpecialization<T>
{
    /// The type of metadata to be used, this tell the pattern framework that this specialization is using custom metadata.
    struct Metadata {};

    /// If Metadata is provided, this function must exists, it is used to convert at compile time the options into metadata.
    template <class Adapter>
    static constexpr Metadata parse(const bzd::StringView options) noexcept { ... }

    template <concepts::inputByteCopyableRange Range>
    static constexpr Optional<Size> fromString(Range&& range, T& value, const Metadata metadata = Metadata{}) noexcept { ... }
};
```
