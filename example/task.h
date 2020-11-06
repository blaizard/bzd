#pragma once

#include "bzd/type_traits/decay.h"
#include "bzd/utility/forward.h"
#include "example/task_interface.h"

namespace bzd {
template <class T>
struct resultOf;

template <class T, class... Args>
struct resultOf<T (&)(Args&&...)>
{
	typedef T type;
};

template <typename Callable>
union storage {
	storage() {}
	bzd::typeTraits::Decay<Callable> callable;
};

template <int, typename Callable, typename Ret, typename... Args>
auto fnptr_(Callable&& c, Ret (*)(Args...))
{
	static bool used = false;
	static storage<Callable> s;
	using type = decltype(s.callable);

	if (used) s.callable.~type();
	new (&s.callable) type(bzd::forward<Callable>(c));
	used = true;

	return [](Args... args) -> Ret { return Ret(s.callable(bzd::forward<Args>(args)...)); };
}

template <typename Fn, int N = 0, typename Callable>
Fn* fnptr(Callable&& c)
{
	return fnptr_<N>(bzd::forward<Callable>(c), (Fn*)nullptr);
}

template <class Callable>
class Task : public interface::Task
{
public:
	Task(Callable&& callable) : interface::Task(makePtr(bzd::forward<Callable>(callable))) {}

	// TODO support the creation of 2 tasks of the same type.
	static FctPtrType makePtr(Callable&& instance)
	{
		using CallableStorageType = bzd::typeTraits::Decay<Callable>;
		static bool used = false;
		static CallableStorageType callable{bzd::forward<Callable>(instance)};
		if (used)
		{
			callable.~CallableStorageType();
			new (&callable) CallableStorageType(bzd::forward<Callable>(instance));
		}
		used = true;
		return []() { callable(); };
	}
};
} // namespace bzd
