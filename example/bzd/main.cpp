#include "bzd/core/registry.h"
#include "bzd/core/system.h"
#include "bzd/format/format.h"

int main()
{
	bzd::format::toString(bzd::getOut(), CONSTEXPR_STRING_VIEW("Message: {}\n"), bzd::Registry<bzd::StringView>::get("message"));

	return 0;
}
