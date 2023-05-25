#pragma once

#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/type_traits/is_pointer.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd {

template <concepts::pointer Pointer>
class NonNullPtr
{
public:
	template <concepts::pointer T>
	constexpr explicit NonNullPtr(T&& pointer) noexcept : pointer_{assignNonNull(bzd::forward<T>(pointer))}
	{
	}

public:
	constexpr NonNullPtr(const NonNullPtr& other) noexcept : pointer_{assignNonNull(other.pointer_)} {}
	constexpr NonNullPtr& operator=(const NonNullPtr& other) noexcept
	{
		pointer_ = assignNonNull(other.pointer_);
		return *this;
	}
	constexpr NonNullPtr(NonNullPtr&& other) noexcept : pointer_{assignNonNull(bzd::move(other.pointer_))} {}
	constexpr NonNullPtr& operator=(NonNullPtr&& other) noexcept
	{
		pointer_ = bzd::move(assignNonNull(other.pointer_));
		return *this;
	}
	~NonNullPtr() = default;

public:
	constexpr auto* operator->() noexcept { return &(*pointer_); }
	constexpr const auto* operator->() const noexcept { return &(*pointer_); }

	constexpr auto& operator*() noexcept { return *pointer_; }
	constexpr const auto& operator*() const noexcept { return *pointer_; }

private:
	template <class T>
	constexpr T&& assignNonNull(T&& pointer) noexcept
	{
		bzd::assert::isTrue(pointer != nullptr);
		return bzd::forward<T>(pointer);
	}

private:
	Pointer pointer_;
};

template <class Pointer>
NonNullPtr(Pointer&&) -> NonNullPtr<Pointer>;

template <class Pointer>
NonNullPtr(Pointer&) -> NonNullPtr<Pointer>;

} // namespace bzd
