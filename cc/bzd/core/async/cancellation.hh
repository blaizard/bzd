#pragma once

#include "cc/bzd/container/non_owning_list.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/core/assert/minimal.hh"
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
public: // Constructors/Destructors.
	constexpr CancellationToken() noexcept = default;

	// Copy/move constructor/assignment
	constexpr CancellationToken(const CancellationToken&) noexcept = delete;
	constexpr CancellationToken& operator=(const CancellationToken&) noexcept = delete;
	constexpr CancellationToken(CancellationToken&&) noexcept = delete;
	constexpr CancellationToken& operator=(CancellationToken&&) noexcept = delete;

	constexpr ~CancellationToken() noexcept
	{
		if (parent_)
		{
			parent_->removeToken(*this);
		}
	}

public: // API.
	/// Attach this token to a parent token that will propagate its triggered
	/// state to this token.
	constexpr void attach(CancellationToken& token) noexcept
	{
		bzd::assert::isTrue(parent_.empty());

		auto scope = makeSyncLockGuard(token.mutex_);
		const auto result = token.attached_.pushFront(*this);
		bzd::assert::isTrue(result.hasValue());
		parent_.emplace(token);
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

	constexpr void removeToken(CancellationToken& token) noexcept
	{
		auto scope = makeSyncLockGuard(mutex_);
		const auto result = attached_.erase(token);
		bzd::assert::isTrue(result.hasValue());
	}

	constexpr BoolType isCanceled() const noexcept { return flag_.load(); }

private:
	bzd::Atomic<BoolType> flag_{false};
	bzd::SpinMutex mutex_{};
	bzd::Optional<CancellationToken&> parent_{};
	bzd::NonOwningList<CancellationToken> attached_{};
};
} // namespace bzd
