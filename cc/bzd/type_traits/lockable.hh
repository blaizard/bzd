#pragma once

namespace bzd::concepts {

/// The BasicLockable concept describes the minimal characteristics of types that provide exclusive blocking
/// semantics for execution agents (i.e. threads).
template <class T>
concept basicLockable = requires(T t)
{
	t.lock();
	t.unlock();
};

template <class T>
concept sharedLockable = requires(T t)
{
	t.lockShared();
	t.unlockShared();
};

} // namespace bzd::concepts
