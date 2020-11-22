#pragma once

#include "bzd/platform/types.h"

namespace bzd::interface {
class Clock
{
public:
	void delay() const noexcept
	{
		if (hasInterrupt_)
		{
			registerHandler();
		}
	}

	virtual bzd::UInt32Type tickToMs() const noexcept = 0;
	// virtual bzd::ClockTickType getTick() const noexcept = 0;
	virtual void registerHandler() noexcept {}

	virtual ~Clock() noexcept = default;
};
} // namespace bzd::interface
  /*
  auto delay(const int timeMs)
  {
	  const auto timestampMs = getTimestampMs();
	  // 3 states: no result, resolve, reject
	  return makePromise<uint64_t, int>([timestampMs, timeMs]() -> PromiseReturnType<uint64_t, int> {
		  const auto currentTimestampMs = getTimestampMs();
		  if (currentTimestampMs < timestampMs + timeMs)
		  {
			  return bzd::nullopt;
		  }
		  return currentTimestampMs;
	  });
  }
  */