#pragma once

#include "cc/bzd/container/non_owning_list.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/synchronization/spin_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

namespace bzd {

/// General purpose timer that can manage multiple alarms.
template <class Time, class Impl, Bool retroactiveAlarm = true, Size bitWidth = 64>
class TimerISR
{
public:
	struct Element : public bzd::NonOwningListElement
	{
		/// The alarm at which time the element should be processed.
		Time alarm;
		/// The suspended executable associated with this element.
		bzd::async::ExecutableSuspended executable;
	};

public:
	bzd::Async<> waitUntil(const Time time) noexcept
	{
		Element element;
		element.alarm = time;
		co_await bzd::async::suspend(
			[&](auto&& executable) {
				element.executable.own(bzd::move(executable));
				add(element);
			},
			[&]() { remove(element); });

		co_return {};
	}

	/// Add a new timer element to the list.
	///
	/// This function is threadsafe but not ISR safe.
	constexpr void add(Element& element) noexcept
	{
		auto lock = makeSyncLockGuard(mutex_);

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
		alarmUpdate();
	}

	/// Remove an element previously added.
	constexpr void remove(Element& element) noexcept
	{
		auto lock = makeSyncLockGuard(mutex_);

		bzd::ignore = list_.erase(element);

		// Start the timer.
		alarmUpdate();
	}

	/// Trigger to be called from an ISR.
	constexpr void triggerForISR() noexcept { executable_.schedule(); }

	/// Check and process all timers that are expired, reload the timer if needed.
	///
	/// This should run as a service in the background.
	bzd::Async<> service() noexcept
	{
		while (true)
		{
			// Set the executable in suspended mode and re-arm the alarm if needed.
			{
				co_await bzd::async::suspendForISR([&](auto&& executable) {
					auto lock = makeSyncLockGuard(mutex_);
					executable_.own(bzd::move(executable));
					alarmUpdate();
				});
			}

			{
				auto lock = makeSyncLockGuard(mutex_);

				// Trigger the alarm expired.
				auto maybeTime = impl().getTime();
				if (!maybeTime)
				{
					co_return bzd::move(maybeTime).propagate();
				}
				for (auto it = list_.begin(); it != list_.end();)
				{
					if (it->alarm <= maybeTime.value())
					{
						it->executable.schedule();
						bzd::ignore = list_.erase(it++);
					}
					else
					{
						break;
					}
				}
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
	bzd::Result<void, bzd::Error> alarmUpdate() noexcept
	{
		const auto maybeAlarm = getNextAlarm();
		if (maybeAlarm)
		{
			auto result = impl().alarmSet(maybeAlarm.value());
			if (!result)
			{
				return bzd::move(result).propagate();
			}
			if constexpr (!retroactiveAlarm)
			{
				// Check if the current time is already passed, if so trigger the callback.
				// This is needed for some architecture that will not trigger the ISR if the
				// alarm is already passed.
				auto maybeTime = impl().getTime();
				if (!maybeTime)
				{
					return bzd::move(maybeTime).propagate();
				}
				if (maybeTime.value() >= maybeAlarm.value())
				{
					executable_.schedule();
				}
			}
			return bzd::nullresult;
		}
		else
		{
			return impl().alarmClear();
		}
	}

	constexpr Impl& impl() noexcept { return *static_cast<Impl*>(this); }
	constexpr const Impl& impl() const noexcept { return *static_cast<const Impl*>(this); }

private:
	SpinMutex mutex_{};
	bzd::NonOwningList<Element> list_{};
	bzd::async::ExecutableSuspendedForISR executable_{};
	Time timeAdd_{0};
};

} // namespace bzd
