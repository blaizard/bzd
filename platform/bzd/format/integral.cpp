#include "bzd/format/integral.h"

namespace bzd { namespace format {

void toString(bzd::OStream& stream, const bzd::StringView& data)
{
	stream.write(data);
}

void toString(bzd::OStream& stream, const char c)
{
	stream.write(bzd::StringView(&c, 1));
}
}} // namespace bzd::format
