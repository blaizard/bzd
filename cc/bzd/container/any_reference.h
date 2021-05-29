#pragma once

#include "cc/bzd/container/result.h"
#include "cc/bzd/core/assert/minimal.h"
#include "cc/bzd/platform/types.h"
#include "cc/bzd/type_traits/is_same.h"

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
	constexpr AnyReference() noexcept = default;

	template <class T>
	constexpr explicit AnyReference(T& object) noexcept : typeId_{bzd::impl::getTypeId<T>()}, object_{&object}
	{
	}

	template <class T>
	constexpr explicit AnyReference(const T& object) noexcept : typeId_{bzd::impl::getTypeId<const T>()}, object_{const_cast<T*>(&object)}
	{
	}

	// Forbid rvalues
	template <class T>
	constexpr explicit AnyReference(T&& object) noexcept = delete;

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
			return *static_cast<const T*>(object_);
		}
		return makeError();
	}

	template <class T>
	[[nodiscard]] constexpr T& cast() noexcept
	{
		auto result = get<T>();
		bzd::assert::isTrue(result.hasValue(), "Invalid any cast");
		return result.valueMutable();
	}

	template <class T>
	[[nodiscard]] constexpr const T& cast() const noexcept
	{
		const auto result = get<T>();
		bzd::assert::isTrue(result.hasValue(), "Invalid any cast");
		return result.value();
	}

private:
	bzd::impl::TypeIdType typeId_{0};
	void* object_{nullptr};
};
} // namespace bzd
