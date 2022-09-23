#pragma once

#include "cc/bzd/container/threadsafe/non_owning_forward_list.hh"
#include "cc/bzd/core/assert.hh"
#include "cc/bzd/platform/stream.hh"
#include "cc/bzd/utility/format/to_string/format.hh"

namespace bzd::test {

class ListElementDiscard : public bzd::threadsafe::NonOwningForwardListElement</*MultiContainer*/ false, /*discard*/ true>
{
public:
	ListElementDiscard() = default;
	ListElementDiscard(bzd::Size value) : value_{value} {}
	ListElementDiscard(ListElementDiscard&&) = default;

	bzd::Size value_{0};
};

class ListElementMultiContainer : public bzd::threadsafe::NonOwningForwardListElement</*MultiContainer*/ true>
{
public:
	ListElementMultiContainer() = default;
	ListElementMultiContainer(bzd::Size value) : value_{value} {}
	ListElementMultiContainer(ListElementMultiContainer&&) = default;

	bzd::Size value_{0};
};

class ListElement : public bzd::threadsafe::NonOwningForwardListElement<false>
{
public:
	ListElement() = default;
	ListElement(bzd::Size value) : value_{value} {}
	ListElement(ListElement&&) = default;

	bzd::Size value_{0};
};

template <class T>
class NonOwningForwardList : public bzd::threadsafe::NonOwningForwardList<T>
{
public:
	using Self = typename bzd::threadsafe::NonOwningForwardList<T>;
	using typename Self::ElementPtrType;

public:
	/**
	 * Ensures that all element are properly connected.
	 * \return The number of node evaluated.
	 */
	template <class U>
	[[nodiscard]] bzd::Size sanityCheck(const U sanityCheckElement = [](const auto&) -> bool { return true; })
	{
		bzd::Size counter = 0;
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
					bzd::assert::isTrue(counter == 0, "Expected 0 size but got: {}"_csv, counter);
				}
				else
				{
					bzd::assert::isTrue(previous == &(maybeBack.value().get()),
										"Last pointer is not pointing to the last element: {} vs {}"_csv,
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
		bzd::assert::isTrue(Self::size() == counter, "Counter ({}) mistmatch with actual size ({})"_csv, counter, Self::size());

		return counter;
	}

	constexpr void printChain(bool onlyIfFails = true, bzd::Size maxElements = 100)
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
			toStream(bzd::platform::out(),
					 "{} {}"_csv,
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
