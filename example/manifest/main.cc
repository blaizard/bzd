#include "bzd-auto/example/manifest/manifest_file.h"

#include <iostream>

class MyType : public MyInterface
{
};

int main()
{
	MyType type{};
	std::cout << "Magic number: " << type.getMagic() << std::endl;
	return 0;
}
