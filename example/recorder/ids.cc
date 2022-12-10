#include "cc/bzd.hh"

namespace bzd {

template <bzd::meta::StringLiteral name>
struct Record
{
	static UInt32 id;
};

template <>
UInt32 Record<"hello">::id = 12u;

} // namespace bzd
