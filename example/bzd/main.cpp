#include "bzd/core/system.h"
#include "bzd/core/registry.h"

int main()
{
	bzd::getOut().write(bzd::StringView{"Message: "});
	bzd::getOut().write(bzd::Registry<bzd::StringView>::get("message"));
	bzd::getOut().write(bzd::StringView{"\n"});

	return 0;
}
