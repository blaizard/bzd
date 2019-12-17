#include "bzd/format/format.h"
#include "bzd/core/system.h"

int main()
{
	bzd::format::toString(bzd::getOut(), CONSTEXPR_STRING_VIEW("The answer is {}.\n"), 42);

	return 0;
}
