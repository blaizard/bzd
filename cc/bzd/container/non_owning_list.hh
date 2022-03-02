#pragma once

#include "cc/bzd/container/result.hh"
#include "cc/bzd/type_traits/add_const.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/is_const.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/utility/ignore.hh"

namespace bzd {
enum class NonOwningListErrorType
{
	elementAlreadyInserted,
	elementAlreadyRemoved,
	empty,
};

struct NonOwningListElement
{
	NonOwningListElement* next_{nullptr};
	NonOwningListElement* previous_{nullptr};
};
} // namespace bzd

namespace bzd::impl {
template <class U>
class NonOwningListIterator
{
public: // Traits
	using Self = NonOwningListIterator<U>;
	using Category = bzd::typeTraits::ForwardTag;
	using IndexType = bzd::SizeType;
	using DifferenceType = bzd::Int32Type;
	using ValueType = U;

private: // Internal Traits.
	using UnderlyingValuePtrType =
		typeTraits::Conditional<bzd::typeTraits::isConst<ValueType>, const NonOwningListElement, NonOwningListElement>*;

public:
	constexpr NonOwningListIterator(UnderlyingValuePtrType ptr) noexcept : current_{ptr} {}

public: // Modifiers.
	constexpr Self& operator++() noexcept
	{
		next();
		return *this;
	}

	constexpr Self operator++(int) noexcept
	{
		Self it{*this};
		++(*this);
		return it;
	}

public: // Comparators.
	[[nodiscard]] constexpr bool operator==(const Self& other) const noexcept { return (current_ == other.current_); }
	[[nodiscard]] constexpr bool operator!=(const Self& other) const noexcept { return !(other == *this); }

public: // Accessors.
	[[nodiscard]] constexpr ValueType& operator*() const { return *static_cast<ValueType*>(current_); }
	[[nodiscard]] constexpr ValueType* operator->() const { return static_cast<ValueType*>(current_); }

private:
	constexpr void next() noexcept
	{
		bzd::assert::isTrue(current_->next_);
		current_ = current_->next_;
	}

private:
	UnderlyingValuePtrType current_;
};
} // namespace bzd::impl

namespace bzd {

template <class T>
class NonOwningList
{
public: // Traits.
	template <class V>
	using Result = bzd::Result<V, NonOwningListErrorType>;
	using Iterator = bzd::impl::NonOwningListIterator<T>;
	using ConstIterator = bzd::impl::NonOwningListIterator<bzd::typeTraits::AddConst<T>>;

public:
	constexpr NonOwningList() noexcept : first_{&last_, nullptr}, last_{nullptr, &first_} {}

public: // Iterator.
	/// Return a begin iterator for this list.
	[[nodiscard]] constexpr auto begin() noexcept { return Iterator{first_.next_}; }
	[[nodiscard]] constexpr auto begin() const noexcept { return ConstIterator{first_.next_}; }

	/// Return an end iterator for this list.
	[[nodiscard]] constexpr auto end() noexcept { return Iterator{&last_}; }
	[[nodiscard]] constexpr auto end() const noexcept { return ConstIterator{&last_}; }

public:
	[[nodiscard]] constexpr bzd::SizeType size() const noexcept { return size_; }

	[[nodiscard]] constexpr bzd::BoolType empty() const noexcept { return (size_ == 0U); }

	constexpr void clear() noexcept
	{
		while (!empty())
		{
			bzd::ignore = popFront();
		}
	}

	[[nodiscard]] constexpr Result<void> pushFront(T& element) noexcept { return insert(first_, element); }

	[[nodiscard]] constexpr Result<void> pushBack(T& element) noexcept { return insert(*last_.previous_, element); }

	[[nodiscard]] constexpr Result<void> insert(NonOwningListElement& previous, T& element) noexcept
	{
		if (element.next_ != nullptr)
		{
			return error(NonOwningListErrorType::elementAlreadyInserted);
		}

		const auto next = previous.next_;
		previous.next_ = &element;
		element.next_ = next;
		element.previous_ = &previous;
		next->previous_ = &element;

		++size_;

		return nullresult;
	}

	[[nodiscard]] constexpr Result<T&> popFront() noexcept
	{
		if (empty())
		{
			return error(NonOwningListErrorType::empty);
		}
		return erase(*(first_.next_));
	}

	[[nodiscard]] constexpr Result<T&> popBack() noexcept
	{
		if (empty())
		{
			return error(NonOwningListErrorType::empty);
		}
		return erase(*(last_.previous_));
	}

	[[nodiscard]] constexpr Result<T&> erase(NonOwningListElement& element) noexcept
	{
		if (element.next_ == nullptr)
		{
			return error(NonOwningListErrorType::elementAlreadyRemoved);
		}

		auto previous = element.previous_;
		previous->next_ = element.next_;
		element.next_->previous_ = previous;
		element.next_ = nullptr;

		--size_;

		return static_cast<T&>(element);
	}

private:
	bzd::SizeType size_{0};
	NonOwningListElement first_;
	NonOwningListElement last_;
};
} // namespace bzd
