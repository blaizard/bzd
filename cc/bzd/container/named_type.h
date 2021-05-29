#pragma once

#include "cc/bzd/type_traits/enable_if.h"
#include "cc/bzd/type_traits/is_arithmetic.h"
#include "cc/bzd/utility/move.h"
#include "cc/bzd/utility/ratio.h"

namespace bzd::impl {
/**
 * Curiously Recuring Template Pattern
 */
template <typename T, template <class> class crtpType>
class NamedTypeCRTP
{
protected:
	constexpr T& underlying() { return static_cast<T&>(*this); }
	constexpr const T& underlying() const { return static_cast<const T&>(*this); }
};

template <class T, typename PhantomType, class Ratio>
class NamedType
{
public: // Constructors.
	constexpr NamedType() noexcept = default;
	explicit constexpr NamedType(const T& value) noexcept : value_{value} {}
	explicit constexpr NamedType(T&& value) noexcept : value_{bzd::move(value)} {}

	// Convert and round the result
	template <class OtherRatio>
	constexpr NamedType(const NamedType<T, PhantomType, OtherRatio>& other) :
		value_{static_cast<T>((other.get() * (Ratio::den * OtherRatio::num) + (Ratio::num * OtherRatio::den) / 2) / (Ratio::num * OtherRatio::den))}
	{
	}

public: // Accessors.
	constexpr T& get() noexcept { return value_; }
	constexpr const T& get() const noexcept { return value_; }

private:
	T value_{};
};

} // namespace bzd::impl

namespace bzd {

/**
 * Strong type.
 */
template <class T, typename PhantomType, template <class> class... Skills>
class NamedType
	: public impl::NamedType<T, PhantomType, bzd::Ratio<1>>
	, public Skills<NamedType<T, PhantomType, Skills...>>...
{
public:
	using UnderlyingType = T;
	using Tag = PhantomType;

public:
	using impl::NamedType<T, PhantomType, bzd::Ratio<1>>::NamedType;
};

/**
 * Creates a multiple of an existing NamedType.
 */
template <class T, class Ratio, template <class> class... Skills>
class MultipleOf
	: public impl::NamedType<typename T::UnderlyingType, typename T::Tag, Ratio>
	, public Skills<MultipleOf<T, Ratio, Skills...>>...
{
public:
	using UnderlyingType = typename T::UnderlyingType;
	using Tag = typename T::Tag;

public:
	using impl::NamedType<UnderlyingType, Tag, Ratio>::NamedType;
};

// ---- Skills

template <class T>
class Incrementable : public impl::NamedTypeCRTP<T, Incrementable>
{
public:
	constexpr T& operator++()
	{
		++this->underlying().get();
		return this->underlying();
	}
	constexpr T operator++(int)
	{
		auto copy = this->underlying();
		this->underlying().get()++;
		return copy;
	}
};

template <class T>
class Decrementable : public impl::NamedTypeCRTP<T, Decrementable>
{
public:
	constexpr T& operator--()
	{
		--this->underlying().get();
		return this->underlying();
	}
	constexpr T operator--(int)
	{
		auto copy = this->underlying();
		this->underlying().get()--;
		return copy;
	}
};

template <class T>
class Addable : public impl::NamedTypeCRTP<T, Addable>
{
public:
	constexpr T operator+(const T& other) const { return T{this->underlying().get() + other.get()}; }
	constexpr T& operator+=(const T& other)
	{
		this->underlying().get() += other.get();
		return this->underlying();
	}
};

template <class T>
class Subtractable : public impl::NamedTypeCRTP<T, Subtractable>
{
public:
	constexpr T operator-(const T& other) const { return T{this->underlying().get() - other.get()}; }
	constexpr T& operator-=(const T& other)
	{
		this->underlying().get() -= other.get();
		return this->underlying();
	}
};

template <class T>
class Multiplicable : public impl::NamedTypeCRTP<T, Multiplicable>
{
public:
	constexpr T operator*(const T& other) const { return T{this->underlying().get() * other.get()}; }
	constexpr T& operator*=(const T& other)
	{
		this->underlying().get() *= other.get();
		return this->underlying();
	}

	template <class U, bzd::typeTraits::EnableIf<typeTraits::isArithmetic<U>, void>* = nullptr>
	constexpr T operator*(const U& value) const
	{
		return T{this->underlying().get() * value};
	}
	template <class U, bzd::typeTraits::EnableIf<typeTraits::isArithmetic<U>, void>* = nullptr>
	constexpr T& operator*=(const U& value)
	{
		this->underlying().get() *= value;
		return this->underlying();
	}
};

template <class T>
class Divisible : public impl::NamedTypeCRTP<T, Divisible>
{
public:
	constexpr T operator/(const T& other) const { return T{this->underlying().get() / other.get()}; }
	constexpr T& operator/=(const T& other)
	{
		this->underlying().get() /= other.get();
		return this->underlying();
	}

	template <class U, bzd::typeTraits::EnableIf<typeTraits::isArithmetic<U>, void>* = nullptr>
	constexpr T operator/(const U& value) const
	{
		return T{this->underlying().get() / value};
	}
	template <class U, bzd::typeTraits::EnableIf<typeTraits::isArithmetic<U>, void>* = nullptr>
	constexpr T& operator/=(const U& value)
	{
		this->underlying().get() /= value;
		return this->underlying();
	}
};

template <class T>
class Modulable : public impl::NamedTypeCRTP<T, Modulable>
{
public:
	constexpr T operator%(const T& other) const { return T{this->underlying().get() % other.get()}; }
	constexpr T& operator%=(const T& other)
	{
		this->underlying().get() %= other.get();
		return this->underlying();
	}

	template <class U, bzd::typeTraits::EnableIf<typeTraits::isArithmetic<U>, void>* = nullptr>
	constexpr T operator%(const U& value) const
	{
		return T{this->underlying().get() % value};
	}
	template <class U, bzd::typeTraits::EnableIf<typeTraits::isArithmetic<U>, void>* = nullptr>
	constexpr T& operator%=(const U& value)
	{
		this->underlying().get() %= value;
		return this->underlying();
	}
};

template <class T>
class BitOperation : public impl::NamedTypeCRTP<T, BitOperation>
{
public:
	constexpr T operator~() const { return T{~this->underlying().get()}; }

	constexpr T operator&(const T& other) const { return T{this->underlying().get() & other.get()}; }
	constexpr T& operator&=(const T& other)
	{
		this->underlying().get() &= other.get();
		return this->underlying();
	}

	constexpr T operator|(const T& other) const { return T{this->underlying().get() | other.get()}; }
	constexpr T& operator|=(const T& other)
	{
		this->underlying().get() |= other.get();
		return this->underlying();
	}

	constexpr T operator^(const T& other) const { return T{this->underlying().get() ^ other.get()}; }
	constexpr T& operator^=(const T& other)
	{
		this->underlying().get() ^= other.get();
		return this->underlying();
	}

	constexpr T operator<<(const T& other) const { return T{this->underlying().get() << other.get()}; }
	constexpr T& operator<<=(const T& other)
	{
		this->underlying().get() <<= other.get();
		return this->underlying();
	}

	template <class U, bzd::typeTraits::EnableIf<typeTraits::isArithmetic<U>, void>* = nullptr>
	constexpr T operator<<(const U& value) const
	{
		return T{this->underlying().get() << value};
	}
	template <class U, bzd::typeTraits::EnableIf<typeTraits::isArithmetic<U>, void>* = nullptr>
	constexpr T& operator<<=(const U& value)
	{
		this->underlying().get() <<= value;
		return this->underlying();
	}

	constexpr T operator>>(const T& other) const { return T{this->underlying().get() >> other.get()}; }
	constexpr T& operator>>=(const T& other)
	{
		this->underlying().get() >>= other.get();
		return this->underlying();
	}

	template <class U, bzd::typeTraits::EnableIf<typeTraits::isArithmetic<U>, void>* = nullptr>
	constexpr T operator>>(const U& value) const
	{
		return T{this->underlying().get() >> value};
	}
	template <class U, bzd::typeTraits::EnableIf<typeTraits::isArithmetic<U>, void>* = nullptr>
	constexpr T& operator>>=(const U& value)
	{
		this->underlying().get() >>= value;
		return this->underlying();
	}
};

template <class T>
class Comparable : public impl::NamedTypeCRTP<T, Comparable>
{
public:
	constexpr bool operator<(const T& other) const { return (this->underlying().get() < other.get()); }
	constexpr bool operator>(const T& other) const { return (this->underlying().get() > other.get()); }
	constexpr bool operator<=(const T& other) const { return (this->underlying().get() <= other.get()); }
	constexpr bool operator>=(const T& other) const { return (this->underlying().get() >= other.get()); }
	constexpr bool operator==(const T& other) const { return (this->underlying().get() == other.get()); }
	constexpr bool operator!=(const T& other) const { return (this->underlying().get() != other.get()); }
};

template <class T>
class Arithmetic
	: public Incrementable<T>
	, public Decrementable<T>
	, public Addable<T>
	, public Subtractable<T>
	, public Multiplicable<T>
	, public Divisible<T>
	, public Modulable<T>
	, public BitOperation<T>
	, public Comparable<T>
{
};
} // namespace bzd
