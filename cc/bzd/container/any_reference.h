#pragma once

#include "bzd/container/result.h"
#include "bzd/platform/types.h"

namespace bzd::impl {
using TypeIdType = IntPtrType;

template <class T>
TypeIdType getTypeId()
{
	static bzd::BoolType dummy = 0;
	return reinterpret_cast<TypeIdType>(&dummy);
}
} // namespace bzd::impl

namespace bzd {
class AnyReference
{
public:
	template <class T>
	constexpr explicit AnyReference(T& object) noexcept : typeId_{bzd::impl::getTypeId<T>()}, object_{&object}
	{
	}

	template <class T>
	constexpr explicit AnyReference(const T& object) noexcept : typeId_{bzd::impl::getTypeId<const T>()}, object_{const_cast<T*>(&object)}
	{
	}

	template <class T>
	[[nodiscard]] constexpr bzd::Result<T&> get() noexcept
	{
		if (bzd::impl::getTypeId<T>() == typeId_)
		{
			return *static_cast<T*>(object_);
		}
		return makeError();
	}

	template <class T>
	[[nodiscard]] constexpr bzd::Result<const T&> get() const noexcept
	{
		if (bzd::impl::getTypeId<T>() == typeId_)
		{
			return *static_cast<T*>(object_);
		}
		return makeError();
	}

private:
	bzd::impl::TypeIdType typeId_;
	void* object_;
};
} // namespace bzd
