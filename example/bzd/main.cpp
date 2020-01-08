#include "bzd.h"

int main()
{
	auto& log = bzd::Registry<bzd::Log>::get("default");
	log.info(CSTR("Message: {}\n"), bzd::Registry<bzd::StringView>::get("message"));

	return 0;
}
