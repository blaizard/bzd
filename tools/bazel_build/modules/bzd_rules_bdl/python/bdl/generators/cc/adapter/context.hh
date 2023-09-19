#pragma once

#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd::generator {

template <class ConfigType>
[[nodiscard]] constexpr auto makeContext(ConfigType&& config_) noexcept
{
	struct
	{
	} stub;
	return makeContext(bzd::forward<ConfigType>(config_), bzd::move(stub));
}

template <class ConfigType, class IOType>
[[nodiscard]] constexpr auto makeContext(ConfigType&& config_, IOType&& io_) noexcept
{
	struct Context
	{
		constexpr Context(ConfigType&& config_, IOType&& io_) noexcept : config{bzd::move(config_)}, io{bzd::move(io_)} {}

		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&&) = delete;
		Context& operator=(Context&&) = delete;
		~Context() = default;

		using Config = ConfigType;
		using IO = IOType;
		const Config config;
		IO io;
	};
	return Context{bzd::move(config_), bzd::move(io_)};
}

} // namespace bzd::generator
