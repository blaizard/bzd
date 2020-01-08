#pragma once

#include "bzd/container/iostream.h"
#include "bzd/container/string.h"
#include "bzd/container/string_view.h"
#include "bzd/core/system.h"
#include "bzd/format/format.h"
#include "bzd/utility/forward.h"

namespace bzd { namespace log {
enum class Level
{
	CRITICAL = 0,
	ERROR = 1,
	WARNING = 2,
	INFO = 3,
	DEBUG = 4
};

template <class... Args>
void print(Args&&... args) noexcept
{
	bzd::format::toString(bzd::getOut(), bzd::forward<Args>(args)...);
}
}} // namespace bzd::log

namespace bzd {
namespace impl {
class Log
{
public:
	constexpr explicit Log(bzd::OStream& out) noexcept : out_(out) {}

	template <class... Args>
	constexpr void info(Args&&... args) noexcept
	{
		print(bzd::log::Level::INFO, bzd::forward<Args>(args)...);
	}

private:
	template <class... Args>
	constexpr void print(const bzd::log::Level /*level*/, Args&&... args) noexcept
	{
		bzd::format::toString(out_, bzd::forward<Args>(args)...);
	}

	bzd::OStream& out_;
};
} // namespace impl

namespace interface {
using Log = impl::Log;
}

class Log : public interface::Log
{
public:
	constexpr Log(bzd::OStream& out) : interface::Log(out) {}
};
} // namespace bzd
