#pragma once

#include "cc/bzd/container/function.h"
#include "cc/bzd/container/variant.h"
#include "cc/bzd/type_traits/add_pointer.h"

namespace bzd::impl {

template <typename Tag, class F, class... Args>
class FunctionView
{
protected:
	using Self = FunctionView<Tag, F, Args...>;
	using ReturnType = F;
	using RawFctPtrType = bzd::typeTraits::AddPointer<ReturnType(Args...)>;
	using BzdFctPtrType = bzd::Function<ReturnType(Args...)>;

public: // Constructor/Assignment
	/**
	 * Constructor from member function.
	 */
	template <class Object, class Member>
	constexpr FunctionView(Object& obj, Member memberPtr) noexcept :
		storage_{
			FunctionMember{&obj, {[memberPtr](void* ptr, Args... args) -> auto {return (reinterpret_cast<Object*>(ptr)->*memberPtr)(args...);
}
}}
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
constexpr explicit FunctionView(T& function) noexcept : FunctionView{function, &T::operator()}
{
}

// Copy constructor/assignment.
constexpr FunctionView(const Self& view) noexcept : storage_{view.storage_} {}
constexpr void operator=(const Self& view) noexcept
{
	storage_ = view.storage_;
}

// Move constructor/assignment.
constexpr FunctionView(Self&& view) noexcept : storage_{bzd::move(view.storage_)} {}
constexpr void operator=(Self&& view) noexcept
{
	storage_ = bzd::move(view.storage_);
}

public:					   // API
template <class... Params> // Needed for perfect forwarding
constexpr ReturnType operator()(Params&&... args) const
{
	if (storage_.template is<FunctionMember>())
	{
		auto& obj = storage_.template get<FunctionMember>().valueMutable();
		return obj.callable_(obj.obj_, bzd::forward<Params>(args)...);
	}
	else
	{
		auto function = storage_.template get<RawFctPtrType>().valueMutable();
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