#pragma once

#include "bzd/utility/move.h"
#include "bzd/utility/ratio.h"

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

template <class T, typename PhantomType, class Ratio, template <class> class... Skills>
class NamedType : public Skills<NamedType<T, PhantomType, Ratio, Skills...>>...
{
private:
	using Self = NamedType<T, PhantomType, Ratio, Skills...>;

public: // Constructors.
	constexpr NamedType() noexcept = default;
	explicit constexpr NamedType(const T& value) noexcept : value_{value} {}
	explicit constexpr NamedType(T&& value) noexcept : value_{bzd::move(value)} {}

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
class NamedType : public impl::NamedType<T, PhantomType, bzd::Ratio<1>, Skills...>
{
public:
	using impl::NamedType<T, PhantomType, bzd::Ratio<1>, Skills...>::NamedType;
	using impl::NamedType<T, PhantomType, bzd::Ratio<1>, Skills...>::operator=;
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
};

template <class T>
class BitOperation : public impl::NamedTypeCRTP<T, BitOperation>
{
public:
    constexpr T operator~() const
    {
        return T{~this->underlying().get()};
    }

    constexpr T operator&(const T& other) const
    {
        return T{this->underlying().get() & other.get()};
    }
    constexpr T& operator&=(const T& other)
    {
        this->underlying().get() &= other.get();
        return this->underlying();
    }

    constexpr T operator|(const T& other) const
    {
        return T{this->underlying().get() | other.get()};
    }
    constexpr T& operator|=(const T& other)
    {
        this->underlying().get() |= other.get();
        return this->underlying();
    }

    constexpr T operator^(const T& other) const
    {
        return T{this->underlying().get() ^ other.get()};
    }
    constexpr T& operator^=(const T& other)
    {
        this->underlying().get() ^= other.get();
        return this->underlying();
    }

    constexpr T operator<<(const T& other) const
    {
        return T{this->underlying().get() << other.get()};
    }
    constexpr T& operator<<=(const T& other)
    {
        this->underlying().get() <<= other.get();
        return this->underlying();
    }

    constexpr T operator>>(const T& other) const
    {
        return T{this->underlying().get() >> other.get()};
    }
    constexpr T& operator>>=(const T& other)
    {
        this->underlying().get() >>= other.get();
        return this->underlying();
    }
};

template <class T>
class Comparable : public impl::NamedTypeCRTP<T, Comparable>
{
public:
    constexpr bool operator<(const T& other) const
    {
        return (this->underlying().get() < other.get());
    }
    constexpr bool operator>(const T& other) const
    {
        return (this->underlying().get() > other.get());
    }
    constexpr bool operator<=(const T& other) const
    {
        return (this->underlying().get() <= other.get());
    }
    constexpr bool operator>=(const T& other) const
    {
        return (this->underlying().get() >= other.get());
    }
    constexpr bool operator==(const T& other) const
    {
        return (this->underlying().get() == other.get());
    }
    constexpr bool operator!=(const T& other) const
    {
        return (this->underlying().get() != other.get());
    }
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
