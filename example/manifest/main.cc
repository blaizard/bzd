#include "example/manifest/manifest.h"

#include <iostream>

#define VIRTUAL_TYPE 1

class MyType : public bzd::manifest::adapter::MyInterface<MyType>
{
public:
	bzd::manifest::Test start(int a) { return (a > 10) ? bzd::manifest::Test::FIRST : bzd::manifest::Test::SECOND; }
};

class Interface
{
#ifdef VIRTUAL_TYPE
public:
	virtual int hello(int a) noexcept = 0;
#endif
};

#ifndef VIRTUAL_TYPE

namespace adapter {

template <class Impl>
class Interface : public ::Interface
{
public:
	constexpr Interface<Impl>& base() noexcept { return *this; }

	constexpr int hello(int a) noexcept
	{
		std::cout << "non-virtual" << std::endl;
		return static_cast<Impl*>(this)->hello(a);
	}
};

} // namespace adapter

// Virtual

#else

template <class Impl>
class InterfaceVirtualImpl : public ::Interface
{
public:
	constexpr InterfaceVirtualImpl(Impl& impl) noexcept : impl_{impl} {}

	int hello(int a) noexcept override
	{
		std::cout << "virtual" << std::endl;
		return impl_.hello(a);
	}

private:
	Impl& impl_;
};

namespace adapter {
template <class Impl>
class Interface
{
public:
	constexpr Interface() noexcept : impl_{static_cast<Impl&>(*this)} {}

	constexpr InterfaceVirtualImpl<Impl>& base() noexcept { return impl_; }

private:
	InterfaceVirtualImpl<Impl> impl_;
};
} // namespace adapter
#endif

// Implementation

class User : public adapter::Interface<User>
{
public:
	constexpr int hello(int a) noexcept { return a; }
};

int main()
{
	User user{};
	std::cout << "Test " << user.hello(23) << std::endl;

	MyType type{};
	std::cout << "Magic number: " << type.getMagic() << std::endl;
	return 0;
}
