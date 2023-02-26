#pragma once

#include "cc/bzd/container/iterator/bidirectional.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/type_traits/add_const.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/is_const.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/utility/ignore.hh"

namespace bzd {
struct NonOwningListElement
{
	NonOwningListElement* next_{nullptr};
	NonOwningListElement* previous_{nullptr};
};
} // namespace bzd

namespace bzd {

template <class T>
class NonOwningList
{
private:
	template <class V, class S>
	struct DefaultPolicies : public bzd::iterator::impl::DefaultPolicies<V>
	{
		using StorageValueType = S;
		static constexpr void increment(auto*& data) noexcept { data = data->next_; }
		static constexpr void decrement(auto*& data) noexcept { data = data->previous_; }
	};

public: // Traits.
	using Value = bzd::typeTraits::AddConst<T>;
	using ValueMutable = T;
	using ConstIterator = bzd::iterator::Bidirectional<Value, void, DefaultPolicies<Value, const NonOwningListElement>>;
	using Iterator = bzd::iterator::Bidirectional<ValueMutable, void, DefaultPolicies<ValueMutable, NonOwningListElement>>;

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
	[[nodiscard]] constexpr bzd::Size size() const noexcept { return size_; }

	[[nodiscard]] constexpr bzd::Bool empty() const noexcept { return (size_ == 0U); }

	constexpr void clear() noexcept
	{
		while (!empty())
		{
			bzd::ignore = popFront();
		}
	}

	/// Prepends the given element value to the beginning of the container.
	///
	/// \param element The element to be added.
	/// \return A reference to the element added in case of success, empty optional otherwise.
	[[nodiscard]] constexpr Optional<T&> pushFront(NonOwningListElement& element) noexcept { return insert(begin(), element); }

	/// Appends the given element value to the end of the container.
	///
	/// \param element The element to be added.
	/// \return A reference to the element added in case of success, empty optional otherwise.
	[[nodiscard]] constexpr Optional<T&> pushBack(NonOwningListElement& element) noexcept { return insert(end(), element); }

	/// Insert the new element before the iterator at the specified position.
	///
	/// \param pos The iterator pointing to the element before which the new element should be inserted.
	/// \param element The element to be added.
	///	\return A reference to the element added in case of success, empty optional otherwise.
	[[nodiscard]] constexpr Optional<T&> insert(Iterator pos, NonOwningListElement& element) noexcept
	{
		if (element.next_ != nullptr)
		{
			return bzd::nullopt;
		}

		auto previous = pos->previous_;
		previous->next_ = &element;
		element.next_ = &(*pos);
		element.previous_ = previous;
		pos->previous_ = &element;

		++size_;

		return static_cast<T&>(element);
	}

	[[nodiscard]] constexpr Optional<T&> front() noexcept
	{
		if (empty())
		{
			return bzd::nullopt;
		}
		return static_cast<T&>(*(first_.next_));
	}

	[[nodiscard]] constexpr Optional<T&> back() noexcept
	{
		if (empty())
		{
			return bzd::nullopt;
		}
		return static_cast<T&>(*(last_.previous_));
	}

	[[nodiscard]] constexpr Optional<T&> popFront() noexcept
	{
		if (empty())
		{
			return bzd::nullopt;
		}
		return erase(*(first_.next_));
	}

	[[nodiscard]] constexpr Optional<T&> popBack() noexcept
	{
		if (empty())
		{
			return bzd::nullopt;
		}
		return erase(*(last_.previous_));
	}

	[[nodiscard]] constexpr Optional<T&> erase(Iterator pos) noexcept { return erase(*pos); }
	[[nodiscard]] constexpr Optional<T&> erase(NonOwningListElement& element) noexcept
	{
		if (element.next_ == nullptr)
		{
			return bzd::nullopt;
		}

		auto previous = element.previous_;
		previous->next_ = element.next_;
		element.next_->previous_ = previous;
		element.next_ = nullptr;

		--size_;

		return static_cast<T&>(element);
	}

	template <class Callable>
	constexpr void removeIf(Callable&& callable) noexcept
	{
		auto current = first_.next_;
		while (current != &last_)
		{
			if (callable(static_cast<T&>(*current)))
			{
				const auto next = current->next_;
				bzd::ignore = erase(*current);
				current = next;
			}
			else
			{
				current = current->next_;
			}
		}
	}

private:
	bzd::Size size_{0};
	NonOwningListElement first_;
	NonOwningListElement last_;
};
} // namespace bzd
