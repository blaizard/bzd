#include "example/manifest/interface.h"

#include <iostream>

class MyType : public bzd::manifest::MyInterface
{
public:
	bzd::manifest::Test start(int a) override { return (a > 10) ? bzd::manifest::Test::FIRST : bzd::manifest::Test::SECOND; }
};

int main()
{
	MyType type{};
	std::cout << "Magic number: " << type.getMagic() << std::endl;
	return 0;
}
