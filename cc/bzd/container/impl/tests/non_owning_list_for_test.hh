#pragma once

#include "cc/bzd/container/impl/non_owning_list.hh"
#include "cc/bzd/core/assert.hh"
#include "cc/bzd/platform/stream.hh"
#include "cc/bzd/utility/format/format.hh"

namespace bzd::test {

class ListElementMultiContainer : public bzd::NonOwningListElement</*MultiContainer*/ true>
{
public:
	ListElementMultiContainer(bzd::SizeType value) : value_{value} {}
	ListElementMultiContainer(ListElementMultiContainer&&) = default;

	bzd::SizeType value_;
};

class ListElement : public bzd::NonOwningListElement<false>
{
public:
	ListElement(bzd::SizeType value) : value_{value} {}
	ListElement(ListElement&&) = default;

	bzd::SizeType value_;
};

template <class T>
class NonOwningList : public bzd::NonOwningList<T>
{
public:
	using Self = typename bzd::NonOwningList<T>;
	using typename Self::ElementPtrType;

public:
	/**
	 * Ensures that all element are properly connected.
	 * \return The number of node evaluated.
	 */
	template <class U>
	[[nodiscard]] bzd::SizeType sanityCheck(const U sanityCheckElement = [](const auto&) -> bool { return true; })
	{
		bzd::SizeType counter = 0;
		auto previous = &this->front_;
		auto node = Self::removeMarks(previous->next_.load());

		printNode(previous);

		while (true)
		{
			bzd::assert::isTrue(!Self::isDeletionMark(node), "Deletion mark");
			bzd::assert::isTrue(!Self::isInsertionMark(node), "Insertion mark");
			bzd::assert::isTrue(node, "Null node");

			printNode(node);

			// Check if the end is reached.
			if (node == &this->back_)
			{
				const auto maybeBack = Self::back();
				if (!maybeBack)
				{
					bzd::assert::isTrue(counter == 0, CSTR("Expected 0 size but got: {}"), counter);
				}
				else
				{
					bzd::assert::isTrue(previous == &(maybeBack.value().get()),
										CSTR("Last pointer is not pointing to the last element: {} vs {}"),
										previous,
										&(maybeBack.value().get()));
				}

				break;
			}

			// Check the element.
			{
				const bool result = sanityCheckElement(reinterpret_cast<T&>(*node));
				bzd::assert::isTrue(result, "Custom sanityCheckElement failed");
			}

			++counter;
			previous = node;
			node = node->next_.load();
		}

		// Element count should be equal.
		bzd::assert::isTrue(Self::size() == counter, CSTR("Counter ({}) mistmatch with actual size ({})"), counter, Self::size());

		return counter;
	}

	constexpr void printChain(bool onlyIfFails = true, bzd::SizeType maxElements = 100)
	{
		ElementPtrType cur = &this->front_;
		while (--maxElements && cur && cur != &this->back_)
		{
			if (!onlyIfFails)
			{
				printNode(cur);
			}
			cur = removeDeletionMark(cur->next_.load());
		}
		if (maxElements == 0)
		{
			if (onlyIfFails)
			{
				printChain(/*onlyIfFails*/ false);
			}
			bzd::assert::unreachable();
		}
	}

	void printNode(ElementPtrType node)
	{
		const auto printAddress = [this](const ElementPtrType address) {
			bzd::format::toStream(bzd::platform::out(),
								  CSTR("{} {}"),
								  address,
								  (address == &this->front_) ? " (F)" : ((address == &this->back_) ? " (B)" : "    "))
				.sync();
		};

		printAddress(node);
		if (node)
		{
			bzd::platform::out().write(bzd::StringView{" -> "}.asBytes()).sync();
			printAddress(node->next_.load());
		}
		bzd::platform::out().write(bzd::StringView{"\n"}.asBytes()).sync();
	}
};
} // namespace bzd::test
