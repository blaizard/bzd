#pragma once

#include "cc/bzd/container/result.hh"

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

template <class T>
class NonOwningList
{
public:
	template <class V>
	using Result = bzd::Result<V, NonOwningListErrorType>;

public:
	constexpr NonOwningList() noexcept : first_{&last_, nullptr}, last_{nullptr, &first_} {}

public:
	[[nodiscard]] constexpr bzd::SizeType size() const noexcept { return size_; }

	[[nodiscard]] constexpr bzd::BoolType empty() const noexcept { return (size_ == 0U); }

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

	[[nodiscard]] constexpr Result<void> popFront() noexcept
	{
		if (empty())
		{
			return error(NonOwningListErrorType::empty);
		}
		return erase(*(first_.next_));
	}

	[[nodiscard]] constexpr Result<void> popBack() noexcept
	{
		if (empty())
		{
			return error(NonOwningListErrorType::empty);
		}
		return erase(*(last_.previous_));
	}

	[[nodiscard]] constexpr Result<void> erase(NonOwningListElement& element) noexcept
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

		return nullresult;
	}

private:
	bzd::SizeType size_{0};
	NonOwningListElement first_;
	NonOwningListElement last_;
};
} // namespace bzd
