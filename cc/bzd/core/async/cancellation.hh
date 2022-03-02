#pragma once

#include "cc/bzd/container/non_owning_list.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/utility/synchronization/spin_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

/// A cancellation token.
///
/// Callgraph representation:
/// C  C  C
///  \ | /
///   \|/
///    C
///    |
///
namespace bzd {
class CancellationToken : public bzd::NonOwningListElement
{
public:
	constexpr CancellationToken() noexcept = default;

	/// Attach this token to a parent token that will propagate its triggered
	/// state to this token.
	constexpr void attach(CancellationToken& token) noexcept
	{
		bzd::assert::isTrue(!parent_);

		auto scope = makeSyncLockGuard(token.mutex_);
		parent_ = token;
		token.attached_.pushFront(*this);
	}

	constexpr ~CancellationToken() noexcept
	{
		if (parent_)
		{
			parent_.value().removeToken(*this);
		}
	}

	constexpr void trigger() noexcept
	{
		flag_.store(true);
		{
			auto scope = makeSyncLockGuard(mutex_);
			for (auto& token : attached_)
			{
				token.trigger();
			}
		}
	}

	constexpr void removeToken(const CancellationToken& token) noexcept
	{
		auto scope = makeSyncLockGuard(mutex_);
		const auto result = attached_.erase(token);
		bzd::assert::isTrue(result);
	}

	constexpr BoolType isCanceled() const noexcept { return flag_.load(); }

private:
	bzd::Atomic<BoolType> flag_{false};
	bzd::SpinMutex mutex_{};
	bzd::Optional<CancellationToken&> parent_{};
	bzd::NonOwningList<CancellationToken> attached_{};
};
} // namespace bzd
