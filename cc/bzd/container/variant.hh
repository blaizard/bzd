#pragma once

#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/meta/type_list.hh"
#include "cc/bzd/meta/union.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_constructible.hh"
#include "cc/bzd/type_traits/remove_reference.hh"

namespace bzd::impl {
template <class... Ts>
class VariantBase
{
protected:
	using Self = VariantBase<Ts...>;
	// Metaprogramming list type
	using TypeList = bzd::meta::TypeList<Ts...>;
	// Choose the Nth element out of the list
	template <SizeType N>
	using ChooseNth = typename TypeList::template ChooseNth<N>;
	// Checks if the type T is contained into the list
	template <class T>
	using Contains = typename TypeList::template Contains<T>;
	// Search for T in the list
	template <class T>
	using Find = typename TypeList::template Find<T>;
	// Search for constructible element from T
	template <class T>
	using FindConstructible = typename TypeList::template FindConditional<T, bzd::typeTraits::IsConstructible1>;

	// Helper
	template <SizeType N, SizeType Max, template <class> class F, class... Args>
	struct HelperT
	{
		static constexpr auto call(const SizeType index, Args&&... args) noexcept
		{
			using T = ChooseNth<N>;
			if (index == N)
			{
				return F<T>::call(bzd::forward<Args>(args)...);
			}
			return HelperT<N + 1, Max, F, Args...>::call(index, bzd::forward<Args>(args)...);
		}
	};

	template <SizeType N, template <class> class F, class... Args>
	struct HelperT<N, N, F, Args...>
	{
		static constexpr auto call(const SizeType index, Args&&... args) noexcept
		{
			using T = ChooseNth<N>;
			bzd::assert::isTrue(index == N, "Inconsistent variant state, should never happen");
			return F<T>::call(bzd::forward<Args>(args)...);
		}
	};

	template <template <class> class F, class... Args>
	using Helper = HelperT<0, sizeof...(Ts) - 1, F, Args...>;

	// Overload
	template <class... Fs>
	struct Overload;

	template <class F0, class... Frest>
	struct Overload<F0, Frest...>
		: F0
		, Overload<Frest...>
	{
		template <class T>
		constexpr Overload(T&& f0, Frest&&... rest) noexcept : F0{bzd::forward<T>(f0)}, Overload<Frest...>{bzd::forward<Frest>(rest)...}
		{
		}
		using F0::operator();
		using Overload<Frest...>::operator();
	};

	template <class F0>
	struct Overload<F0> : F0
	{
		template <class T>
		constexpr Overload(T&& f0) noexcept : F0{bzd::forward<T>(f0)}
		{
		}
		using F0::operator();
	};

	// Match
	template <class T>
	struct VariantMatch
	{
		template <class SelfType, class V>
		static constexpr void call(SelfType& self, V& visitor) noexcept
		{
			visitor(self.data_.template get<T>());
		}
	};
	template <class V, class SelfType>
	using Match = Helper<VariantMatch, SelfType&, V&>;

	// Copy visitor
	struct CopyVisitor
	{
		constexpr CopyVisitor(const VariantBase<Ts...>& variant) noexcept : variant_{variant} {}
		template <class T>
		constexpr void operator()(T& value) noexcept
		{
			value = variant_.get<T>();
		}

	private:
		const VariantBase<Ts...>& variant_;
	};

public:
	/**
	 * Default constructor
	 */
	constexpr VariantBase() noexcept = default;

	/**
	 * Value constructor (exact type match)
	 */
	template <class T, int Index = Find<bzd::typeTraits::RemoveReference<T>>::value, bzd::typeTraits::EnableIf<Index != -1>* = nullptr>
	constexpr VariantBase(T&& value) noexcept : id_{Index}, data_{bzd::forward<T>(value)}
	{
	}

	/**
	 * Value constructor (lazy, if constructible)
	 */
	/*	template <class T, int Index = FindConstructible<bzd::typeTraits::RemoveReference<T>>::value,
	   bzd::typeTraits::EnableIf<Find<T>::value == -1 && Index != -1>* = nullptr> constexpr VariantBase(T&& value) noexcept : id_{Index},
	   data_{static_cast<ChooseNth<Index>>(value)}
		{
		}
	*/

	/**
	 * Copy constructor
	 */
	constexpr VariantBase(const Self& variant) noexcept { *this = variant; }

	constexpr Self& operator=(const Self& variant) noexcept
	{
		CopyVisitor visitor{variant};
		id_ = variant.id_;
		Match<CopyVisitor, decltype(*this)>::call(id_, *this, visitor);

		return *this;
	}

	constexpr bzd::Int16Type index() const noexcept { return id_; }

	template <class T>
	constexpr bzd::BoolType is() const noexcept
	{
		return (id_ != -1 && id_ == Find<T>::value);
	}

	template <class T>
	constexpr const T& get() const noexcept
	{
		return data_.template get<T>();
	}

	template <class T>
	constexpr T& get() noexcept
	{
		return data_.template get<T>();
	}

	template <class... Functors>
	constexpr void match(Functors&&... funcs) const noexcept
	{
		const Overload<bzd::typeTraits::RemoveReference<Functors>...> visitor{bzd::forward<Functors>(funcs)...};
		Match<decltype(visitor), decltype(*this)>::call(id_, *this, visitor);
	}

protected:
	bzd::Int16Type id_{-1};
	bzd::meta::Union<Ts...> data_{};
};

template <class... Ts>
class VariantTrivial : public VariantBase<Ts...>
{
public:
	using Self = VariantTrivial<Ts...>;
	using Parent = VariantBase<Ts...>;
	using Parent::VariantBase;
};

template <class... Ts>
class VariantNonTrivial : public VariantBase<Ts...>
{
protected:
	using Self = VariantNonTrivial<Ts...>;
	using Parent = VariantBase<Ts...>;

	// Choose the Nth element out of the list
	template <SizeType N>
	using ChooseNth = typename Parent::template ChooseNth<N>;
	// Checks if the type T is contained into the list
	template <class T>
	using Contains = typename Parent::template Contains<T>;
	// Search for T in the list
	template <class T>
	using Find = typename Parent::template Find<T>;
	// Helper class
	template <template <class> class F, class... Args>
	using Helper = typename Parent::template Helper<F, Args...>;

	// Data types
	using Parent::data_;
	using Parent::id_;

protected:
	// Destructor
	template <class T>
	struct VariantDestructor
	{
		static void call(Self* self) noexcept { self->data_.template get<T>().~T(); }
	};
	using Destructor = Helper<VariantDestructor, Self*>;

private:
	template <class T, class... Args, bzd::typeTraits::EnableIf<Contains<T>::value>* = nullptr>
	constexpr void construct(Args&&... args) noexcept
	{
		static_assert(Find<T>::value != -1, "Inconsistent variant state, should never happen");
		// Using inplace operator new
		::new (&(data_.template get<T>())) T(bzd::forward<Args>(args)...);
		// Sets the ID only if the constructor succeeded
		id_ = Find<T>::value;
	}

	constexpr void destructIfNeeded() noexcept
	{
		if (id_ != -1)
		{
			Destructor::call(id_, this);
		}
	}

public:
	using Parent::VariantBase;

	constexpr Self& operator=(const Self& other) noexcept
	{
		destructIfNeeded();
		Parent::operator=(other);
		return *this;
	}
	/*
		constexpr Self& operator=(Self&& other) noexcept
		{
			destructIfNeeded();
			return Parent::operator=(other);
		}
	*/
	template <class T, class... Args, bzd::typeTraits::EnableIf<Contains<T>::value>* = nullptr>
	constexpr void emplace(Args&&... args) noexcept
	{
		destructIfNeeded();
		construct<T>(bzd::forward<Args>(args)...);
	}

	~VariantNonTrivial() noexcept { destructIfNeeded(); }
};
} // namespace bzd::impl

namespace bzd {
template <class... Ts>
using Variant = bzd::typeTraits::
	Conditional<(bzd::typeTraits::isTriviallyDestructible<Ts> && ...), impl::VariantTrivial<Ts...>, impl::VariantNonTrivial<Ts...>>;
} // namespace bzd
