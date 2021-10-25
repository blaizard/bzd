#pragma once

#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/meta/type_list.hh"
#include "cc/bzd/meta/union.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_trivially_copy_assignable.hh"
#include "cc/bzd/type_traits/is_trivially_copy_constructible.hh"
#include "cc/bzd/type_traits/is_trivially_destructible.hh"
#include "cc/bzd/type_traits/is_trivially_move_assignable.hh"
#include "cc/bzd/type_traits/is_trivially_move_constructible.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/forward.hh"
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

	// A protected tag to select a special constructor to create an empty variant.
	// After this call the variant will be in an invalid state.
	struct EmptyConstructorTagType
	{
	};
	constexpr VariantBase(EmptyConstructorTagType) noexcept : id_{npos}, data_{} {}

public: // Constructors
	/// Default constructor.
	/// The first type is selected, this is compatible with std::variant behavior.
	constexpr VariantBase() noexcept : id_{0}, data_{InPlaceIndex<0>{}} {}

	/// Value constructor (exact type match)
	template <class T>
	constexpr VariantBase(T&& value) noexcept : id_{Find<bzd::typeTraits::RemoveReference<T>>::value}, data_{bzd::forward<T>(value)}
	{
	}

	/// Value constructor, in place index constructor.
	template <SizeType I, class... Args>
	constexpr VariantBase(InPlaceIndex<I>, Args&&... args) noexcept : id_{I}, data_{inPlaceIndex<I>, bzd::forward<Args>(args)...}
	{
	}

	/// Value constructor, in place type constructor.
	template <class T, class... Args>
	constexpr VariantBase(InPlaceType<T>, Args&&... args) noexcept :
		id_{Find<bzd::typeTraits::RemoveReference<T>>::value}, data_{inPlaceType<T>, bzd::forward<Args>(args)...}
	{
	}

	// Default copy/move constructors/assignments
	constexpr VariantBase(const Self& variant) noexcept = default;
	constexpr Self& operator=(const Self& variant) noexcept = default;
	constexpr VariantBase(Self&& variant) noexcept = default;
	constexpr Self& operator=(Self&& variant) noexcept = default;

public: // Functions
	template <class T, class... Args, bzd::typeTraits::EnableIf<Contains<T>::value>* = nullptr>
	constexpr void emplace(Args&&... args) noexcept
	{
		// Using inplace operator new
		::new (&(data_.template get<T>())) T(bzd::forward<Args>(args)...);
		// Sets the ID only if the constructor succeeded
		id_ = Find<T>::value;
	}

	[[nodiscard]] constexpr IndexType index() const noexcept { return id_; }

	template <class T>
	[[nodiscard]] constexpr bzd::BoolType is() const noexcept
	{
		return (id_ == Find<T>::value);
	}

	template <class T>
	[[nodiscard]] constexpr const T& get() const noexcept
	{
		return data_.template get<T>();
	}

	template <class T>
	[[nodiscard]] constexpr T& get() noexcept
	{
		return data_.template get<T>();
	}

	template <class T>
	constexpr void set(T&& value) noexcept
	{
		id_ = Find<bzd::typeTraits::RemoveReference<T>>::value;
		data_ = bzd::forward<T>(value);
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
	// ... [ Note: If any non-static data member of a union has a non-trivial default constructor (12.1),
	// copy constructor (12.8), move constructor (12.8), copy assignment operator (12.8), move assignment
	// operator (12.8), or destructor (12.4), the corresponding member function of the union must be user-provided
	// or it will be implicitly deleted (8.4.3) for the union. —end note ]
	bzd::meta::Union<Ts...> data_{};
};

// Copy constructible ---------------------------------------------------------

template <class... Ts>
class VariantTriviallyCopyConstructible : public VariantBase<Ts...>
{
public: // Traits / Constructors
	using Self = VariantTriviallyCopyConstructible<Ts...>;
	using Parent = VariantBase<Ts...>;
	using Parent::Parent;
};

template <class... Ts>
class VariantNonTriviallyCopyConstructible : public VariantBase<Ts...>
{
public: // Traits
	using Self = VariantNonTriviallyCopyConstructible<Ts...>;
	using Parent = VariantBase<Ts...>;

protected:
	// Copy constructor visitor
	struct CopyConstructorVisitor
	{
		constexpr CopyConstructorVisitor(const VariantNonTriviallyCopyConstructible<Ts...>& variant) noexcept : variant_{variant} {}
		template <class T, class SelfType>
		constexpr void operator()(SelfType& self) noexcept
		{
			self.template emplace<T>(variant_.template get<T>());
		}

	private:
		const VariantNonTriviallyCopyConstructible<Ts...>& variant_;
	};

	template <class V, class SelfType>
	using Match = typename Parent::template Match<V, SelfType>;
	template <class T>
	using Find = typename Parent::template Find<T>;

public: // Constructors / Assignments
	using Parent::Parent;

	constexpr VariantNonTriviallyCopyConstructible(const Self& variant) noexcept : Parent{typename Parent::EmptyConstructorTagType{}}
	{
		*this = variant;
	}
	constexpr Self& operator=(const Self& variant) noexcept
	{
		this->id_ = variant.id_;
		CopyConstructorVisitor visitor{variant};
		Match<CopyConstructorVisitor, decltype(*this)>::call(this->id_, *this, visitor);

		return *this;
	}
	constexpr VariantNonTriviallyCopyConstructible(Self&& variant) noexcept = default;
	constexpr Self& operator=(Self&& variant) noexcept = default;
};

template <class... Ts>
using VariantCopyConstructible = bzd::typeTraits::Conditional<(typeTraits::isTriviallyCopyConstructible<Ts> && ...) &&
																  (typeTraits::isTriviallyCopyAssignable<Ts> && ...),
															  VariantTriviallyCopyConstructible<Ts...>,
															  VariantNonTriviallyCopyConstructible<Ts...>>;

// Move constructible ---------------------------------------------------------

template <class... Ts>
class VariantTriviallyMoveConstructible : public VariantCopyConstructible<Ts...>
{
public: // Traits / Constructors
	using Self = VariantTriviallyMoveConstructible<Ts...>;
	using Parent = VariantCopyConstructible<Ts...>;
	using Parent::Parent;
};

template <class... Ts>
class VariantNonTriviallyMoveConstructible : public VariantCopyConstructible<Ts...>
{
public: // Traits
	using Self = VariantNonTriviallyMoveConstructible<Ts...>;
	using Parent = VariantCopyConstructible<Ts...>;

protected:
	// Move constructor visitor
	struct MoveConstructorVisitor
	{
		constexpr MoveConstructorVisitor(VariantNonTriviallyMoveConstructible<Ts...>& variant) noexcept : variant_{variant} {}
		template <class T, class SelfType>
		constexpr void operator()(SelfType& self) noexcept
		{
			self.template emplace<T>(bzd::move(variant_.template get<T>()));
		}

	private:
		VariantNonTriviallyMoveConstructible<Ts...>& variant_;
	};

	template <class V, class SelfType>
	using Match = typename Parent::template Match<V, SelfType>;
	template <class T>
	using Find = typename Parent::template Find<T>;

public: // Constructors / Assignments
	using Parent::Parent;

	constexpr VariantNonTriviallyMoveConstructible(const Self& variant) noexcept = default;
	constexpr Self& operator=(const Self& variant) noexcept = default;
	constexpr VariantNonTriviallyMoveConstructible(Self&& variant) noexcept : Parent{typename Parent::EmptyConstructorTagType{}}
	{
		*this = bzd::move(variant);
	}
	constexpr Self& operator=(Self&& variant) noexcept
	{
		this->id_ = variant.id_;
		MoveConstructorVisitor visitor{variant};
		Match<MoveConstructorVisitor, decltype(*this)>::call(this->id_, *this, visitor);

		return *this;
	}
};

template <class... Ts>
using VariantMoveConstructible = bzd::typeTraits::Conditional<(typeTraits::isTriviallyMoveConstructible<Ts> && ...) &&
																  (typeTraits::isTriviallyMoveAssignable<Ts> && ...),
															  VariantTriviallyMoveConstructible<Ts...>,
															  VariantNonTriviallyMoveConstructible<Ts...>>;

// Variant --------------------------------------------------------------------

template <class... Ts>
class VariantTrivial : public VariantMoveConstructible<Ts...>
{
public: // Traits
	using Self = VariantTrivial<Ts...>;
	using Parent = VariantMoveConstructible<Ts...>;
	using Parent::Parent;
	using Parent::operator=;

	template <class T>
	using Find = typename Parent::template Find<T>;

	template <class T, int index = Find<bzd::typeTraits::RemoveReference<T>>::value, bzd::typeTraits::EnableIf<index != -1>* = nullptr>
	constexpr Self& operator=(T&& value) noexcept
	{
		this->set(bzd::forward<T>(value));
		return *this;
	}
};

template <class... Ts>
class VariantNonTrivial : public VariantMoveConstructible<Ts...>
{
public: // Traits
	using Self = VariantNonTrivial<Ts...>;
	using Parent = VariantMoveConstructible<Ts...>;

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

protected:
	// Destructor
	template <class T>
	struct VariantDestructor
	{
		static void call(Self* self) noexcept { self->data_.template get<T>().~T(); }
	};
	using Destructor = Helper<VariantDestructor, Self*>;

private:
	constexpr void destructIfNeeded() noexcept { Destructor::call(this->id_, this); }

public:
	using Parent::Parent;

	constexpr VariantNonTrivial(const Self& variant) noexcept = default;
	constexpr VariantNonTrivial(Self&& variant) noexcept = default;

	template <class T>
	constexpr Self& operator=(T&& other) noexcept
	{
		destructIfNeeded();
		if constexpr (Find<bzd::typeTraits::RemoveReference<T>>::value == -1)
		{
			Parent::operator=(bzd::forward<T>(other));
		}
		else
		{
			this->set(bzd::forward<T>(other));
		}
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
