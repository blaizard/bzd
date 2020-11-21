#pragma once

#include "bzd/utility/move.h"

namespace bzd::impl {
/**
 * Curiously Recuring Template Pattern
 */
template <typename T, template <class> class crtpType>
class NamedTypeCRTP
{
protected:
    T& underlying()
    {
        return static_cast<T&>(*this);
    }
    const T& underlying() const
    {
        return static_cast<const T&>(*this);
    }
};
}

namespace bzd {

/**
 * Strong type.
 * 
 * Implementation inspired from: https://github.com/joboccara/NamedType/blob/master/include/NamedType/
 */
template <class T, typename PhantomType, template <class> class... Skills>
class NamedType : public Skills<NamedType<T, PhantomType, Skills...>>...
{
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

// ---- Skills

template <class T>
class Incrementable : public impl::NamedTypeCRTP<T, Incrementable>
{
public:
    T& operator++()
    {
        ++this->underlying().get();
        return this->underlying();
    }
    T operator++(int)
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
    T& operator--()
    {
        --this->underlying().get();
        return this->underlying();
    }
    T operator--(int)
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
	T operator+(const T& other) const { return T(this->underlying().get() + other.get()); }
    T& operator+=(const T& other)
    {
        this->underlying().get() += other.get();
        return this->underlying();
    }
};

template <class T>
class Subtractable : public impl::NamedTypeCRTP<T, Subtractable>
{
public:
    T operator-(const T& other) const { return T(this->underlying().get() - other.get()); }
    T& operator-=(const T& other)
    {
        this->underlying().get() -= other.get();
        return this->underlying();
    }
};
   
template <class T>
class Multiplicable : public impl::NamedTypeCRTP<T, Multiplicable>
{
public:
    T operator*(const T& other) const
    {
        return T(this->underlying().get() * other.get());
    }
    T& operator*=(const T& other)
    {
        this->underlying().get() *= other.get();
        return this->underlying();
    }
};

template <class T>
class Divisible : public impl::NamedTypeCRTP<T, Divisible>
{
public:
    T operator/(const T& other) const
    {
        return T(this->underlying().get() / other.get());
    }
    T& operator/=(const T& other)
    {
        this->underlying().get() /= other.get();
        return this->underlying();
    }
};

template <class T>
class Modulable : public impl::NamedTypeCRTP<T, Modulable>
{
public:
    T operator%(const T& other) const
    {
        return T(this->underlying().get() % other.get());
    }
    T& operator%=(const T& other)
    {
        this->underlying().get() %= other.get();
        return this->underlying();
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
{
};
}
