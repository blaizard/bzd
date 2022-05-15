#include "cc/bzd/type_traits/function.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "example/manifest/manifest.hh"

#include <iostream>

// ------------- NEW version ----------------

template <class Impl>
class Hello
{
protected:
	// With this the class cannot be instanciated directly.
	constexpr Hello() noexcept
	{
		using bzd::typeTraits::isSame;
		static_assert(isSame<typename Traits::world::Signature, typename TraitsImpl::world::Signature> && TraitsImpl::world::isNoexcept,
					  "Wrong signature for member 'world', expecting: void world() noexcept;");
	}

public:
	void world() noexcept
	{
		std::cout << "before" << std::endl;
		static_cast<Impl*>(this)->world();
		std::cout << "after" << std::endl;
	}

private: // Traits.
	using Self = Hello<Impl>;
	struct TraitsImpl
	{
		using world = bzd::typeTraits::Function<decltype(&Impl::world)>;
	};
	struct Traits
	{
		using world = bzd::typeTraits::Function<decltype(&Self::world)>;
	};
};

class HelloImpl : public Hello<HelloImpl>
{
public:
	HelloImpl(std::string name) : name_{name} {}

	void world() const noexcept { std::cout << "Hello " << name_ << std::endl; }

private:
	std::string name_;
};

int main()
{
	// The composition will instanciate the implementation.
	// At this point, checks on the impl are made.
	HelloImpl hello{"Mister"};

	// The composition will instiante and call object passing the interface.
	// This in addition to use the interface, ensure that the class implements it.
	auto& interface = static_cast<decltype(hello)::Hello&>(hello);

	// The interace function is used.
	interface.world();

	return 0;
}

/*
What are the interface bdl supposed to do:

1. Generate the user facing API.
2. Able to wrap the underlying implementation call (to be able to add synchronization primitives for example).
3. Provide definition of types and constants.
4. Check implementation compliances.
5. Use the interface object to interact.

interface Hello
{
	method world();
	// no variable, it's up to you how variable are implemented, only functions should be exposed.
}

// Auto generates C++:

template <class Impl>
class Hello
{
	concepts conceptWorld = ...;
	static_assert(conceptWorld<Impl>, "dsds");
public:
	void world()
	{
		impl.hello();
	}

private:
	Impl& impl;
};

// User implementation (for example):

class HelloImpl : public Hello<HelloImpl>
{
public:
	void world()
	{
		std::cout << message << std::endl;
	}

private:
	std::string message{"Hello World!"};
};

*/
