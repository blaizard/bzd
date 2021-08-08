#include "example/manifest/manifest.h"

#include <iostream>

class MyType : public bzd::manifest::MyInterface<MyType>
{
public:
	bzd::manifest::Test start(int a) override { return (a > 10) ? bzd::manifest::Test::FIRST : bzd::manifest::Test::SECOND; }
};

// Non virtual

template <class Impl>
class Interface
{
public:
	static constexpr bool isBaseVirtual = false;
	static constexpr bool isBaseStatic = true;

public:
	constexpr Interface<Impl>& base() noexcept { return *this; }

	// static_assert(Impl::hello exsits);

	constexpr int hello(int a) noexcept
	{
		std::cout << "non-virtual" << std::endl;
		return static_cast<Impl*>(this)->hello(a);
	}
};

// Virtual

class InterfaceVirtualBase
{
public:
	virtual int hello(int a) noexcept = 0;
};

template <class Impl>
class InterfaceVirtualImpl : public InterfaceVirtualBase
{
public:
	static constexpr bool isBaseVirtual = true;
	static constexpr bool isBaseStatic = false;

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

template <class Impl>
class InterfaceVirtual
{
public:
	static constexpr bool isBaseVirtual = true;
	static constexpr bool isBaseStatic = false;

public:
	constexpr InterfaceVirtual() noexcept : impl_{static_cast<Impl&>(*this)} {}

	constexpr InterfaceVirtualImpl<Impl>& base() noexcept { return impl_; }

	// static_assert(Impl::hello exsits);

private:
	InterfaceVirtualImpl<Impl> impl_;
};

// Implementation

class User : public InterfaceVirtual<User>
{
public:
	constexpr int hello(int a) noexcept { return a; }
};

int main()
{
	User user{};
	std::cout << "Test " << user.base().hello(23) << std::endl;

	MyType type{};
	std::cout << "Magic number: " << type.getMagic() << std::endl;
	return 0;
}
