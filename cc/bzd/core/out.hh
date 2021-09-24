#pragma once

#include "cc/bzd/core/channel.hh"

namespace bzd {
class Out
{
public:
	static bzd::OStream& getDefault() noexcept;
	static void setDefault(bzd::OStream& out) noexcept;
};
} // namespace bzd
