#pragma once

#include "cc/bzd/container/variant.hh"
#include "cc/bzd/type_traits/add_pointer.hh"
#include "cc/bzd/type_traits/invocable.hh"
#include "cc/bzd/type_traits/is_same_class.hh"

namespace bzd {

template <class... Args>
class FunctionRef;

/// Handles 2 types of functions:
/// - member functions / lambdas.
/// - raw function pointers / static members.
template <class R, class... Args>
class FunctionRef<R(Args...)>
{
public: // Traits.
	using Self = FunctionRef<R(Args...)>;
	using ReturnType = R;
	using FunctionPointer = bzd::typeTraits::AddPointer<ReturnType(Args...)>;
	struct FunctionMember
	{
		ReturnType (*wrapper_)(void*, Args...);
		void* typeErasedObject_{nullptr};
	};

public: //  Constructors/Factories.
	// Lambda (const and mutable).
	template <class T>
	requires(!concepts::sameClassAs<T, Self> &&
			 concepts::invocableR<T, ReturnType, Args...>) constexpr explicit FunctionRef(T& lambda) noexcept
	{
		*this = toMember<T, &T::operator()>(lambda);
	}

	// Function pointer.
	constexpr explicit FunctionRef(FunctionPointer pointer) noexcept : storage_{pointer} {}

	// Pointer to member (mutable).
	template <class Object, ReturnType (Object::*memberPtr)(Args...)>
	static constexpr auto toMember(Object& object) noexcept
	{
		FunctionMember storage{wrapper<Object, memberPtr>, &object};
		return FunctionRef{storage};
	}

	// Pointer to member (const).
	template <class Object, ReturnType (Object::*memberPtr)(Args...) const>
	static constexpr auto toMember(const Object& object) noexcept
	{
		FunctionMember storage{wrapperConst<Object, memberPtr>, const_cast<void*>(static_cast<const void*>(&object))};
		return FunctionRef{storage};
	}

public:						   // API.
	template <class... Params> // Needed for perfect forwarding
	constexpr ReturnType operator()(Params&&... params) const
	{
		if (storage_.template is<FunctionMember>())
		{
			auto& storage = storage_.template get<FunctionMember>();
			return storage.wrapper_(storage.typeErasedObject_, bzd::forward<Params>(params)...);
		}
		auto function = storage_.template get<FunctionPointer>();
		return function(bzd::forward<Params>(params)...);
	}

private: // Private constructor.
	constexpr explicit FunctionRef(FunctionMember storage) noexcept : storage_{storage} {}

private: // Static wrappers.
	template <class Object, ReturnType (Object::*memberPtr)(Args...)>
	static ReturnType wrapper(void* typeErasedObject, Args... args)
	{
		auto object = static_cast<Object*>(typeErasedObject);
		return (object->*memberPtr)(args...);
	}

	template <class Object, ReturnType (Object::*memberPtr)(Args...) const>
	static ReturnType wrapperConst(void* typeErasedObject, Args... args)
	{
		auto object = static_cast<const Object*>(typeErasedObject);
		return (object->*memberPtr)(args...);
	}

private: // Members.
	bzd::Variant<FunctionMember, FunctionPointer> storage_;
};

} // namespace bzd
