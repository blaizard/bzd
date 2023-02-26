#pragma once

#include "cc/bzd/container/non_owning_list.hh"
#include "cc/bzd/utility/synchronization/spin_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

namespace bzd {

/// General purpose timer that can manage multiple alarms.
template <class Time, class Duration, class Impl>
class Timer
{
public:
	struct Element : public bzd::NonOwningListElement
	{
		/// The alarm at which time the element should be processed.
		Time alarm;
	};

public:
	void timerStart(const Time alarm)
	{
		// call the actual function to start the timer
		nextAlarm_ = alarm;
	}

	void timerStop()
	{
		// call the actual function to stop the timer
		nextAlarm_.clear();
	}

public:
	/// Add a new timer element to the list.
	///
	/// This function is threadsafe but not ISR safe.
	constexpr void add(Element& element, const Duration duration) noexcept
	{
		element.alarm = impl().durationToTime(duration);
		auto lock = makeSyncLockGuard(mutex_);

		// If the timer is running, stop it. This ensures
		// that no ISR will be triggered when we insert the new element.
		if (nextAlarm_)
		{
			timerStop();
		}

		// Add the new element, sorted by alarm time.
		auto it = list_.begin();
		for (; it != list_.end(); ++it)
		{
			if (element.alarm <= it->alarm)
			{
				break;
			}
		}
		bzd::ignore = list_.insert(it, element);

		// Start the timer.
		timerStartIfNeeded();
	}

	/// Remove an element previously added.
	constexpr void remove(Element& element) noexcept
	{
		auto lock = makeSyncLockGuard(mutex_);

		// If the timer is running, stop it. This ensures
		// that no ISR will be triggered when we insert the new element.
		if (nextAlarm_)
		{
			timerStop();
		}

		bzd::ignore = list_.erase(element);

		// Start the timer.
		timerStartIfNeeded();
	}

	/// Trigger to be called from an ISR.
	constexpr void triggerForISR() noexcept
	{
		nextAlarm_.clear();
		executable_.schedule();
	}

	/// Check and process all timers that are expired, reload the timer if needed.
	///
	/// This should run as a service in the background.
	bzd::Async<> service() noexcept
	{
		while (true)
		{
			co_await bzd::async::suspendForISR([&](auto&& executable) { executable_ = bzd::move(executable); });

			{
				auto lock = makeSyncLockGuard(mutex_);

				// Trigger the alarm expired.
				const auto time = impl().getTime();
				for (auto it = list_.begin(); it != list_.end();)
				{
					if (it->alarm <= time)
					{
						bzd::ignore = list_.erase(it++);
					}
					else
					{
						break;
					}
				}

				// Start the timer if needed.
				timerStartIfNeeded();
			}
		}
		co_return {};
	}

private:
	[[nodiscard]] constexpr bzd::Optional<Time> getNextAlarm() noexcept
	{
		const auto& element = list_.front();
		if (!element)
		{
			return bzd::nullopt;
		}
		return element->alarm;
	}

	/// Arm the timer if needed.
	constexpr void timerStartIfNeeded() noexcept
	{
		const auto maybeAlarm = getNextAlarm();
		if (maybeAlarm)
		{
			timerStart(maybeAlarm.value());
		}
	}

	constexpr Impl& impl() noexcept { return *static_cast<Impl*>(this); }
	constexpr const Impl& impl() const noexcept { return *static_cast<const Impl*>(this); }

private:
	SpinMutex mutex_{};
	bzd::NonOwningList<Element> list_{};
	bzd::Optional<Time> nextAlarm_{};
	bzd::async::ExecutableSuspendedForISR executable_{};
};

} // namespace bzd
