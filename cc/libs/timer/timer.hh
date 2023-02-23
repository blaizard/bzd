#pragma once

#include "cc/bzd/container/non_owning_list.hh"
#include "cc/bzd/utility/synchronization/spin_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

namespace bzd {

/// Timer that manage different time and alarms.
template <class Time>
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
	constexpr void add(Element& element) noexcept
	{
		auto lock = makeSyncLockGuard(mutex_);

		// If the timer is running, stop it. This ensures
		// that no ISR will be triggered when we insert the new element.
		if (nextAlarm_)
		{
			timerStop();
		}

		// Add the new element, sort by alarm.
		auto it = list_.begin();
		for (; it != list_.last(); ++it)
		{
			if (element.alarm <= it->alarm)
			{
				list_.insert(it, element);
				break;
			}
		}

		// Start the alarm.
		timerStart(getNextAlarm());
	}

	/// Check the time and process all timers that are expired.
	///
	/// This function is ISR safe.
	constexpr void process() noexcept {}

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

private:
	SpinMutex mutex_{};
	bzd::NonOwningList<Element> list_{};
	bzd::Optional<Time> nextAlarm_{};
};

} // namespace bzd
