#pragma once

#include "include/types.h"
#include "include/utility.h"
#include "include/template_metaprogramming.h"
#include "include/assert_minimal.h"
#include "include/container/expected.h"

namespace bzd
{
	template <class... Ts>
	class Variant
	{
	private:
    	using Self = Variant<Ts...>;
		// Metaprogramming list type
		using TypeList = bzd::tmp::TypeList<Ts...>;
		// Choose the Nth element out of the list
		template <SizeType N>
		using ChooseNth = typename TypeList::template ChooseNth<N>;
		// Checks if the type T is contained into the list
		template <class T>
		using Contains = typename TypeList::template Contains<T>;
		// Search for T in the list
		template <class T>
		using Find = typename TypeList::template Find<T>;

		// Destructor
        template <class T>
        struct VariantDestructor
		{
            static void call(Self* self)
			{
				reinterpret_cast<T*>(&(self->data_))->~T();
            }
        };

		// Helper
        template <SizeType N, SizeType Max, template<class> class F, class... Args>
        struct HelperT
		{
            static auto call(const SizeType index, Args&&... args)
			{
                using T = ChooseNth<N>;
                if (index == N)
				{
                    return F<T>::call(bzd::forward<Args>(args)...);
                } 
				return HelperT<N + 1, Max, F, Args...>::call(index, bzd::forward<Args>(args)...);
            }
        };

        template <SizeType N, template<class> class F, class... Args>
        struct HelperT<N, N, F, Args...>
		{
            static auto call(const SizeType index, Args&&... args)
			{
                using T = ChooseNth<N>;
				bzd::assertTrue(index == N, "Inconsistent variant state, should never happen");
                return F<T>::call(bzd::forward<Args>(args)...);
            }
        };

    	template <template<class> class F, class... Args>
        using Helper = HelperT<0, sizeof...(Ts) - 1, F, Args...>;

		using Destructor = Helper<VariantDestructor, Self*>;

		using StorageType = typename bzd::alignedStorage<bzd::tmp::variadicMax(sizeof(Ts)...), bzd::tmp::variadicMax(alignof(Ts)...)>::type;

	private:

		template <class T, class... Args, typename bzd::typeTraits::enableIf<Contains<T>::value>::type* = nullptr>
		constexpr void construct(Args&&... args)
		{
			static_assert(Find<T>::value != -1, "Inconsistent variant state, should never happen");
			// Using inplace operator new
			::new (&data_) T(bzd::forward<Args>(args)...);
			// Sets the ID only if the constructor succeeded
			id_ = Find<T>::value;
		}

		constexpr void destructIfNeeded()
		{
			if (id_ != -1)
			{
				Destructor::call(id_, this);
			}
		}

	public:
		constexpr Variant() = default;

		template <class T, typename bzd::typeTraits::enableIf<Contains<T>::value>::type* = nullptr>
		constexpr Variant(T&& value)
		{
			construct<T>(bzd::forward<T>(value));
		}

		template <class T, class... Args, typename bzd::typeTraits::enableIf<Contains<T>::value>::type* = nullptr>
		constexpr void emplace(Args&&... args)
		{
			destructIfNeeded();
			construct<T>(bzd::forward<Args>(args)...);
		}

		~Variant()
		{
			destructIfNeeded();
		}

		template <class T>
		constexpr bool is() const noexcept
		{
			return (id_ != -1 && id_ == Find<T>::value);
		}

		template <class T>
		constexpr bzd::Expected<const T&, bool> get() const
		{
			if (!is<T>())
			{
				return bzd::makeUnexpected(false);
			}
			return *reinterpret_cast<T*>(&data_);
		}

		template <class T>
		constexpr bzd::Expected<T&, bool> get()
		{
			if (!is<T>())
			{
				return bzd::makeUnexpected(false);
			}
			return *reinterpret_cast<T*>(&data_);
		}

	protected:
		int id_ = -1;
		StorageType data_ = {};
	};
}
