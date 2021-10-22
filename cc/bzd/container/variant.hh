#pragma once

#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/meta/type_list.hh"
#include "cc/bzd/meta/union.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_constructible.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd::impl {
template <class... Ts>
class VariantBase
{
protected:
	using Self = VariantBase<Ts...>;
	// Type use to define the index of the variant type.
	using IndexType = bzd::Int16Type;
	// Position of the variant representing an invalid state.
	static constexpr const IndexType npos = static_cast<IndexType>(-1);
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

	// MatchValue
	template <class T>
	struct VariantMatchValue
	{
		template <class SelfType, class V>
		static constexpr void call(SelfType& self, V& visitor) noexcept
		{
			visitor(self.data_.template get<T>());
		}
	};
	template <class V, class SelfType>
	using MatchValue = Helper<VariantMatchValue, SelfType&, V&>;

	// Match
	template <class T>
	struct VariantMatch
	{
		template <class SelfType, class V>
		static constexpr void call(SelfType& self, V& visitor) noexcept
		{
			visitor.template operator()<T>(self);
		}
	};
	template <class V, class SelfType>
	using Match = Helper<VariantMatch, SelfType&, V&>;

	// Copy constructor visitor
	struct CopyConstructorVisitor
	{
		constexpr CopyConstructorVisitor(const VariantBase<Ts...>& variant) noexcept : variant_{variant} {}
		template <class T, class SelfType>
		constexpr void operator()(SelfType& self) noexcept
		{
			self.template emplace<T>(variant_.get<T>());
		}

	private:
		const VariantBase<Ts...>& variant_;
	};

	// Move constructor visitor
	struct MoveConstructorVisitor
	{
		constexpr MoveConstructorVisitor(VariantBase<Ts...>& variant) noexcept : variant_{variant} {}
		template <class T, class SelfType>
		constexpr void operator()(SelfType& self) noexcept
		{
			self.template emplace<T>(bzd::move(variant_.get<T>()));
		}

	private:
		VariantBase<Ts...>& variant_;
	};

	// A protected tag to select a special constructor to create an empty variant.
	// After this call the variant will be in an invalid state.
	struct EmptyConstructorTagType
	{
	};
	constexpr VariantBase(EmptyConstructorTagType) noexcept : id_{npos}, data_{} {}

public:
	/**
	 * Default constructor.
	 * The first type is selected, this is compatible with std::variant behavior.
	 */
	constexpr VariantBase() noexcept : id_{0}, data_{InPlaceIndex<0>{}} {}

	/**
	 * Value constructor (exact type match)
	 */
	template <class T>
	constexpr VariantBase(T&& value) noexcept : id_{Find<bzd::typeTraits::RemoveReference<T>>::value}, data_{bzd::forward<T>(value)}
	{
	}

	/**
	 * Value constructor, in place index constructor.
	 */
	template <SizeType I, class... Args>
	constexpr VariantBase(InPlaceIndex<I>, Args&&... args) noexcept : id_{I}, data_{inPlaceIndex<I>, bzd::forward<Args>(args)...}
	{
	}

	/**
	 * Value constructor, in place type constructor.
	 */
	template <class T, class... Args>
	constexpr VariantBase(InPlaceType<T>, Args&&... args) noexcept :
		id_{Find<bzd::typeTraits::RemoveReference<T>>::value}, data_{inPlaceType<T>, bzd::forward<Args>(args)...}
	{
	}

	/**
	 * Value assignment (copy/move)
	 */
	template <class T, int index = Find<bzd::typeTraits::RemoveReference<T>>::value, bzd::typeTraits::EnableIf<index != -1>* = nullptr>
	constexpr Self& operator=(T& value) noexcept
	{
		id_ = index;
		data_ = bzd::forward<T>(value);
		return *this;
	}

	/**
	 * Copy constructor.
	 */
	constexpr VariantBase(const Self& variant) noexcept { *this = variant; }

	/**
	 * Copy assignment.
	 */
	constexpr Self& operator=(const Self& variant) noexcept
	{
		id_ = variant.id_;
		CopyConstructorVisitor visitor{variant};
		Match<CopyConstructorVisitor, decltype(*this)>::call(id_, *this, visitor);

		return *this;
	}

	/**
	 * Move constructor.
	 */
	constexpr VariantBase(Self&& variant) noexcept { *this = bzd::move(variant); }

	/**
	 * Move assignment.
	 */
	constexpr Self& operator=(Self&& variant) noexcept
	{
		id_ = variant.id_;
		MoveConstructorVisitor visitor{variant};
		Match<MoveConstructorVisitor, decltype(*this)>::call(id_, *this, visitor);

		return *this;
	}

	template <class T, class... Args, bzd::typeTraits::EnableIf<Contains<T>::value>* = nullptr>
	constexpr void emplace(Args&&... args) noexcept
	{
		static_assert(Find<T>::value != -1, "Inconsistent variant state, should never happen");
		// Using inplace operator new
		::new (&(data_.template get<T>())) T(bzd::forward<Args>(args)...);
		// Sets the ID only if the constructor succeeded
		id_ = Find<T>::value;
	}

	constexpr IndexType index() const noexcept { return id_; }

	template <class T>
	constexpr bzd::BoolType is() const noexcept
	{
		return (id_ == Find<T>::value);
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
		MatchValue<decltype(visitor), decltype(*this)>::call(id_, *this, visitor);
	}

protected:
	IndexType id_;
	// In order to be constexpr compatible, the use of union here is a must because constexpr functions
	// cannot use new nor reinterpret_cast.
	bzd::meta::Union<Ts...> data_{};
};

template <class... Ts>
class VariantTrivial : public VariantBase<Ts...>
{
public: // Traits
	using Self = VariantTrivial<Ts...>;
	using Parent = VariantBase<Ts...>;
	using Parent::VariantBase;
};

template <class... Ts>
class VariantNonTrivial : public VariantBase<Ts...>
{
public: // Traits
	using Self = VariantNonTrivial<Ts...>;
	using Parent = VariantBase<Ts...>;

protected:
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
	constexpr void destructIfNeeded() noexcept { Destructor::call(id_, this); }

public:
	using Parent::VariantBase;

	constexpr VariantNonTrivial(const Self& variant) noexcept : Parent{typename Parent::EmptyConstructorTagType{}}
	{
		Parent::operator=(variant);
	}

	constexpr VariantNonTrivial(Self&& variant) noexcept : Parent{typename Parent::EmptyConstructorTagType{}}
	{
		Parent::operator=(bzd::move(variant));
	}

	template <class T>
	constexpr Self& operator=(T&& other) noexcept
	{
		destructIfNeeded();
		Parent::operator=(bzd::forward<T>(other));
		return *this;
	}

	template <class T, class... Args>
	constexpr void emplace(Args&&... args) noexcept
	{
		destructIfNeeded();
		Parent::template emplace<T>(bzd::forward<Args>(args)...);
	}

	~VariantNonTrivial() noexcept { destructIfNeeded(); }
};
} // namespace bzd::impl

namespace bzd {
template <class... Ts>
using Variant = bzd::typeTraits::
	Conditional<(bzd::typeTraits::isTriviallyDestructible<Ts> && ...), impl::VariantTrivial<Ts...>, impl::VariantNonTrivial<Ts...>>;
} // namespace bzd
