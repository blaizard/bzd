#pragma once

#include "include/types.h"
#include "include/utility.h"
#include "include/template_metaprogramming.h"

namespace bzd
{
	template <class... Ts>
	class Variant
	{
	private:
		// Metaprogramming list type
		using TypeList = bzd::tmp::TypeList<Ts...>;
		// Checks if the type T is contained into the list
		template <class T>
		using Contains = typename TypeList::template Contains<T>;

		using StorageType = typename bzd::alignedStorage<bzd::tmp::variadicMax(sizeof(Ts)...), bzd::tmp::variadicMax(alignof(Ts)...)>::type;

	public:
		constexpr Variant() = default;
	
		template <class T, typename bzd::typeTraits::enableIf<Contains<T>::value>::type* = nullptr>
		constexpr Variant(T&& value)
		{
			// using inplace operator new
			::new (reinterpret_cast<T*>(&data_)) T(bzd::forward<T>(value));
		}

	protected:
		StorageType data_ = {};
	};
}
