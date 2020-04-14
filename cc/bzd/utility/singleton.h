#pragma once

namespace bzd {
template <class T>
class Singleton
{
public:
	static T& getInstance()
	{
		static T instance;
		return instance;
	}
	Singleton(Singleton const&) = delete;
	void operator=(Singleton const&) = delete;

protected:
	Singleton() = default;
};

template <class T>
class SingletonThreadLocal
{
public:
	static T& getInstance()
	{
		static thread_local T instance;
		return instance;
	}
	SingletonThreadLocal(SingletonThreadLocal const&) = delete;
	void operator=(SingletonThreadLocal const&) = delete;

protected:
	SingletonThreadLocal() = default;
};
} // namespace bzd
