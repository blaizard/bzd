#pragma once

#include "bzd/container/function.h"
#include "bzd/container/variant.h"
#include "bzd/type_traits/add_pointer.h"

namespace bzd::impl {

template <typename Tag, class F, class... Args>
class FunctionPointer
{
protected:
	using ReturnType = F;
	using RawFctPtrType = bzd::typeTraits::AddPointer<ReturnType(Args...)>;
	using BzdFctPtrType = bzd::Function<ReturnType(Args...)>;

public:
	/**
	 * Points to a member function.
	 */
	template <class Object, class T>
	constexpr FunctionPointer(Object& obj, T memberPtr) noexcept :
		storage_{
			FunctionMember{&obj, [memberPtr](void* ptr, Args... args) -> auto {return (reinterpret_cast<Object*>(ptr)->*memberPtr)(args...);
}
} // namespace bzd::impl
}
{
}

constexpr explicit FunctionPointer(RawFctPtrType function) noexcept : storage_{function} {}

constexpr explicit FunctionPointer(BzdFctPtrType function) noexcept : FunctionPointer{static_cast<RawFctPtrType>(function)} {}

template <class... Params> // Needed for perfect forwarding
constexpr ReturnType operator()(Params&&... args) const
{
	if (storage_.template is<FunctionMember>())
	{
		auto& obj = *storage_.template get<FunctionMember>();
		return obj.callable_(obj.obj_, bzd::forward<Params>(args)...);
	}
	else
	{
		auto function = *storage_.template get<RawFctPtrType>();
		return function(bzd::forward<Params>(args)...);
	}
}

private:
struct FunctionMember
{
	void* obj_;
	bzd::Function<ReturnType(void*, Args...), Tag> callable_;
};
bzd::Variant<FunctionMember, RawFctPtrType> storage_;
}
;

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