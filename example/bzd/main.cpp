#include "bzd.h"

int main()
{
	// Get the default logger
	auto& log = bzd::Registry<bzd::Log>::get("default");

	// Log a new message
	log.info(CSTR("Message: {}\n"), bzd::Registry<bzd::StringView>::get("message"));

	return 0;
}
