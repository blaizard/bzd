#pragma once

#include "bzd/container/function.h"
#include "bzd/container/variant.h"
#include "bzd/type_traits/add_pointer.h"

namespace bzd::impl {

template <typename Tag, class F, class... Args>
class FunctionView
{
protected:
	using ReturnType = F;
	using RawFctPtrType = bzd::typeTraits::AddPointer<ReturnType(Args...)>;
	using BzdFctPtrType = bzd::Function<ReturnType(Args...)>;

public:
	/**
	 * Constructor from member function.
	 */
	template <class Object, class Member>
	constexpr FunctionView(Object& obj, Member memberPtr) noexcept :
		storage_{
			FunctionMember{&obj, [memberPtr](void* ptr, Args... args) -> auto {return (reinterpret_cast<Object*>(ptr)->*memberPtr)(args...);
}
} // namespace bzd::impl
}
{
}

/**
 * Constructor from function pointer.
 */
constexpr explicit FunctionView(RawFctPtrType function) noexcept : storage_{function} {}

/**
 * Constructor from bzd::Function object.
 */
constexpr explicit FunctionView(BzdFctPtrType& function) noexcept : FunctionView{function.callable_} {}

/**
 * Constructor from lambda.
 */
template <class T>
constexpr explicit FunctionView(T& function) noexcept : FunctionView{function, &T::operator()} {}

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

struct FunctionViewTag
{
};

} // namespace bzd::impl

namespace bzd {

template <class, typename Tag = impl::FunctionViewTag>
class FunctionView;
template <class F, class... ArgTypes>
class FunctionView<F(ArgTypes...)> : public impl::FunctionView<impl::FunctionViewTag, F, ArgTypes...>
{
public:
	using impl::FunctionView<impl::FunctionViewTag, F, ArgTypes...>::FunctionView;
};
template <typename Tag, class F, class... ArgTypes>
class FunctionView<F(ArgTypes...), Tag> : public impl::FunctionView<Tag, F, ArgTypes...>
{
public:
	using impl::FunctionView<Tag, F, ArgTypes...>::FunctionView;
};

} // namespace bzd