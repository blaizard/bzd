#include "cc/bzd/core/out.hh"

namespace {
class Stub : public bzd::OStream
{
public:
	bzd::Async<bzd::SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override { co_return data.size(); }
};

Stub stub;
bzd::OStream* defaultOut{&stub};
} // namespace

void bzd::Out::setDefault(bzd::OStream& out) noexcept
{
	defaultOut = &out;
}

bzd::OStream& bzd::Out::getDefault() noexcept
{
	return *defaultOut;
}
