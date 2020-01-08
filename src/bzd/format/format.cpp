#include "bzd/format/format.h"

namespace bzd { namespace format { namespace impl {

/*
constexpr bool parseUnsignedInteger(bzd::StringView &format, bzd::SizeType &integer)
{
	bool isDefined = false;
	integer = 0;
	for (; format.size() > 0 && format.front() >= '0' && format.front() <= '9';)
	{
		isDefined = true;
		integer = integer * 10 + (format.front() - '0');
		format.removePrefix(1);
	}
	return isDefined;
}

void printString(bzd::OStream &stream, const bzd::StringView stringView, const Metadata &metadata)
{
	switch (metadata.format)
	{
		case Metadata::Format::AUTO:
			stream.write((metadata.isPrecision) ? stringView.subStr(0, bzd::min(metadata.precision, stringView.size())) : stringView);
			break;
		case Metadata::Format::FIXED_POINT:
		case Metadata::Format::FIXED_POINT_PERCENT:
		case Metadata::Format::DECIMAL:
		case Metadata::Format::BINARY:
		case Metadata::Format::HEXADECIMAL_LOWER:
		case Metadata::Format::HEXADECIMAL_UPPER:
		case Metadata::Format::OCTAL:
		case Metadata::Format::POINTER:
			break;
	}
}

void print(bzd::OStream &stream, const bzd::StringView &format, const bzd::interface::Vector<bzd::format::impl::Arg> &args)
{
	Context<PrintContext> ctx(stream, args);
	parse(ctx, format, args);
}
*/
}}} // namespace bzd::format::impl
