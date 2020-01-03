#pragma once

#include "bzd/container/iostream.h"
#include "bzd/container/string.h"
#include "bzd/container/string_view.h"

namespace bzd {
namespace impl {
class StringStream : public bzd::OStream
{
  public:
	constexpr StringStream(bzd::interface::String &str) : string_(str) {}

	SizeType write(const bzd::StringView &data) noexcept { return write(static_cast<const bzd::Span<const char>>(data)); }

	SizeType write(const bzd::Span<const char> &data) noexcept override { return string_.append(data.data(), data.size()); }

	constexpr const bzd::interface::String &str() const noexcept { return string_; }

	constexpr bzd::interface::String &str() noexcept { return string_; }

  protected:
	bzd::interface::String &string_;
};
}  // namespace impl

namespace interface {
using StringStream = bzd::impl::StringStream;
}

template <SizeType N>
class StringStream : public bzd::interface::StringStream
{
  public:
	constexpr StringStream() : bzd::interface::StringStream(container_), container_() {}

  private:
	bzd::String<N> container_;
};
}  // namespace bzd
