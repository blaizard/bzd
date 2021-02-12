#pragma once

#include "bzd/container/function.h"
#include "bzd/utility/ignore.h"

namespace bzd::impl {

template <typename Tag, class F, class... Args>
class FunctionPointer
{
protected:
	using ReturnType = F;

public:
	/**
	 * Points to a member function.
	 */
	template <class Object, class T>
	constexpr FunctionPointer(Object& obj, T memberPtr) :
		obj_{&obj}, callable_{[memberPtr](void* ptr, Args... args) -> auto { return (reinterpret_cast<Object*>(ptr)->*memberPtr)(args...); }}
	{
	}

	/**
	 * Points to a member function, supporting tags.
	 */
	template <class Object, class T>
	constexpr FunctionPointer(Object& obj, T memberPtr, Tag uid) :
		obj_{&obj}, callable_{[memberPtr, uid](void* ptr, Args... args) {
			bzd::ignore = uid;
			(reinterpret_cast<Object*>(ptr)->*memberPtr)(args...);
		}}
	{
	}

	/*	template <class T>
		constexpr FunctionPointer(T* fctPtr) : obj_{nullptr}, callable_{[](void*) {
			(reinterpret_cast<Object*>(ptr)->*memberPtr)();
		}}
		{
		}
	*/

	template <class... Params> // Needed for perfect forwarding
	ReturnType operator()(Params&&... args) const
	{
		return callable_(obj_, bzd::forward<Params>(args)...);
	}

private:
	void* obj_;
	bzd::Function<ReturnType(void*, Args...)> callable_;
};

struct FunctionPointerTag
{
};

} // namespace bzd::impl

namespace bzd {

template <class, typename Tag = impl::FunctionPointerTag>
class FunctionPointer;
template <class F, class... ArgTypes>
class FunctionPointer<F(ArgTypes...)> : public impl::FunctionPointer<impl::FunctionPointerTag, F, ArgTypes...>
{
public:
	using impl::FunctionPointer<impl::FunctionPointerTag, F, ArgTypes...>::FunctionPointer;
};
template <typename Tag, class F, class... ArgTypes>
class FunctionPointer<F(ArgTypes...), Tag> : public impl::FunctionPointer<Tag, F, ArgTypes...>
{
public:
	using impl::FunctionPointer<Tag, F, ArgTypes...>::FunctionPointer;
};

} // namespace bzd