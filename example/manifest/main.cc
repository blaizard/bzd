#include "bzd-auto/example/manifest/manifest_file.h"

#include <iostream>

class MyType : public bzd::manifest::MyInterface
{
public:
	int start(int a) override { return a; }
};

int main()
{
	MyType type{};
	std::cout << "Magic number: " << type.getMagic() << std::endl;
	return 0;
}
