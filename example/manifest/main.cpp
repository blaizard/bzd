#include "bzd-auto/example/manifest/manifest_file.h"

#include <iostream>

class MyType : public MyInterface
{
public:
	uint16_t getMagic() const { return magic; }
};

int main()
{
	MyType type{};
	std::cout << "Magic number: " << type.getMagic() << std::endl;
	return 0;
}
