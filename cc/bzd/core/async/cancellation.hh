#pragma once

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
class CancellationToken
{
public:
	constexpr CancellationToken() noexcept = default;

	/// Attach this token to a parent token that will propagate its triggered
	/// state to this token.
	constexpr void attach(CancellationToken& token) noexcept
	{
		bzd::assert::isTrue(parent_ == nullptr);

		auto scope = makeSyncLockGuard(token.mutex_);
		parent_ = &token;

		// Add this token to the linked list.
		auto next = token.attached_;
		token.attached_ = this;
		next_ = next;
	}

	constexpr ~CancellationToken() noexcept
	{
		if (parent_)
		{
			parent_->removeToken(*this);
		}
	}

	constexpr void trigger() noexcept
	{
		flag_.store(true);
		{
			auto scope = makeSyncLockGuard(mutex_);
			auto next = attached_;
			while (next)
			{
				next->trigger();
				next = next->next_;
			}
		}
	}

	constexpr void removeToken(const CancellationToken& token) noexcept
	{
		auto scope = makeSyncLockGuard(mutex_);
		auto next = attached_;
		while (next)
		{
			if (next == &token)
			{
				// TODO FIX
			}
			next = next->next_;
		}
	}

	constexpr BoolType isCanceled() const noexcept { return flag_.load(); }

private:
	bzd::Atomic<BoolType> flag_{false};
	bzd::SpinMutex mutex_{};
	CancellationToken* next_{nullptr};
	CancellationToken* previous_{nullptr};
	CancellationToken* attached_{nullptr};
};
} // namespace bzd
