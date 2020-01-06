#include "bzd.h"

int main()
{
	bzd::format::toString(bzd::getOut(), CONSTEXPR_STRING_VIEW("Message: {}\n"), bzd::Registry<bzd::StringView>::get("message"));

	return 0;
}
