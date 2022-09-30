#include <iostream>

// interface
class Interface
{
public:
	int myFunc(int a);
};

// Use case, 1 interface, 1 implementation -> constexpr, can be done with lto
// Use case, 1 interface, X implementation(s)
class ModuleImplementation : public Interface
{
public:
	int myFunc(int a) { return a + a_; }

private:
	const int a_ = 32;
};

int Interface::myFunc(int a) { return static_cast<ModuleImplementation&>(*this).myFunc(a); }

void callFct(Interface& helper) { std::cout << helper.myFunc(10) << std::endl; }

int main()
{
	ModuleImplementation impl{};

	callFct(impl);

	return 0;
}
