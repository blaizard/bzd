#pragma once

#include "cc/bzd/container/function_ref.hh"
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

class CancellationCallback : public bzd::NonOwningListElement
{
public:
	template <class CallableRef>
	explicit CancellationCallback(CallableRef&& callback) : callback_{bzd::move(callback)}
	{
	}
	CancellationCallback(const CancellationCallback&) = delete;
	CancellationCallback& operator=(const CancellationCallback&) = delete;
	CancellationCallback(CancellationCallback&&) = delete;
	CancellationCallback& operator=(CancellationCallback&&) = delete;
	~CancellationCallback() = default;

	constexpr void operator()() const noexcept { callback_(); }

private:
	bzd::FunctionRef<void(void)> callback_;
};

class CancellationToken : public bzd::NonOwningListElement
{
public: // Constructors/Destructors.
	constexpr CancellationToken() noexcept = default;

	// Copy/move constructor/assignment
	constexpr CancellationToken(const CancellationToken&) noexcept = delete;
	constexpr CancellationToken& operator=(const CancellationToken&) noexcept = delete;
	constexpr CancellationToken(CancellationToken&&) noexcept = delete;
	constexpr CancellationToken& operator=(CancellationToken&&) noexcept = delete;

public: // API.
	/// Attach this token to a parent token that will propagate its triggered
	/// state to this token.
	constexpr void attachTo(CancellationToken& parent) noexcept
	{
		bzd::assert::isTrue(parent_.empty());

		auto scope = makeSyncLockGuard(parent.mutex_);
		const auto result = parent.children_.pushFront(*this);
		bzd::assert::isTrue(result.hasValue());
		parent_.emplace(parent);
	}

	/// Detach a previously attached cancellation token.
	/// If the token was not attached, do nothing.
	constexpr void detach() noexcept
	{
		if (parent_)
		{
			parent_->removeToken(*this);
			parent_.reset();
		}
	}

	constexpr void trigger() noexcept
	{
		auto scope = makeSyncLockGuard(mutex_);
		flag_.store(true);
		// Trigger and pop the callbacks if any.
		{
			auto scope = makeSyncLockGuard(mutexCallbacks_);
			while (true)
			{
				auto maybeCallback = callbacks_.popBack();
				if (!maybeCallback)
				{
					break;
				}
				maybeCallback.value()();
			}
		}
		// Trigger the children cancellation tokens.
		for (auto& token : children_)
		{
			token.trigger();
		}
	}

	constexpr void removeToken(CancellationToken& token) noexcept
	{
		auto scope = makeSyncLockGuard(mutex_);
		const auto result = children_.erase(token);
		bzd::assert::isTrue(result.hasValue());
	}

	constexpr Bool isCanceled() const noexcept { return flag_.load(); }

	constexpr void addOneTimeCallback(CancellationCallback& callback) noexcept
	{
		auto scope = makeSyncLockGuard(mutex_);
		// If the token was already cancelled, only call the callback
		if (isCanceled())
		{
			callback();
		}
		else
		{
			auto scope = makeSyncLockGuard(mutexCallbacks_);
			const auto result = callbacks_.pushFront(callback);
			bzd::assert::isTrue(result.hasValue());
		}
	}

	constexpr void removeCallback(CancellationCallback& callback) noexcept
	{
		auto scope = makeSyncLockGuard(mutexCallbacks_);
		bzd::ignore = callbacks_.erase(callback);
	}

private:
	bzd::Atomic<Bool> flag_{false};
	bzd::SpinMutex mutex_{};
	bzd::Optional<CancellationToken&> parent_{};
	bzd::NonOwningList<CancellationToken> children_{};
	bzd::SpinMutex mutexCallbacks_{};
	bzd::NonOwningList<CancellationCallback> callbacks_{};
};
} // namespace bzd
