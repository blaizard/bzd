#include "bzd.h"

int main()
{
	// bzd::Log log(bzd::getOut());

	auto& log = bzd::Registry<bzd::Log>::get("default");

	log.info(CONSTEXPR_STRING_VIEW("Message: {}\n"), bzd::Registry<bzd::StringView>::get("message"));

	return 0;
}
