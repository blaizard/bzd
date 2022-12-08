#include "cc/bzd/container/range/subrange.hh"

#include "cc/bzd/container/vector.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/range.hh"

TEST(SubRange, Simple)
{
	bzd::Vector<bzd::UInt32, 12> vector;
	bzd::range::SubRange emptyVectorView{vector.begin(), vector.end()};

	static_assert(bzd::concepts::begin<decltype(emptyVectorView)>, "SubRange must validate the `begin` concept.");
	static_assert(bzd::concepts::end<decltype(emptyVectorView)>, "SubRange must validate the `end` concept.");
	static_assert(bzd::concepts::size<decltype(emptyVectorView)>, "SubRange must validate the `size` concept.");
	static_assert(bzd::concepts::range<decltype(emptyVectorView)>, "SubRange must be considered as a range.");

	EXPECT_TRUE(emptyVectorView.empty());
	EXPECT_EQ(emptyVectorView.size(), 0);

	vector.pushBack(1u);
	vector.pushBack(2u);
	vector.pushBack(3u);
	bzd::range::SubRange nonEmptyVectorView{vector.begin(), vector.end()};
	EXPECT_FALSE(nonEmptyVectorView.empty());
	EXPECT_EQ(nonEmptyVectorView.size(), 3);
}

template <class T>
concept hasSize = requires(T& t) { t.size(); };

template <class T>
concept hasData = requires(T& t) { t.data(); };

template <class T>
concept hasRandomAccessor = requires(T& t) { t[0]; };

TEST(SubRange, VariousTypes)
{
	{
		bzd::test::Range<bzd::typeTraits::IteratorCategory::input> range;
		bzd::range::SubRange subView{range.begin(), range.end()};
		static_assert(!bzd::concepts::sizedRange<decltype(subView)>, "No size member.");
		static_assert(!hasSize<decltype(subView)>, "No size member.");
		static_assert(!hasData<decltype(subView)>, "No data member.");
		static_assert(!hasData<const decltype(subView)>, "No const data member.");
		static_assert(!hasRandomAccessor<decltype(subView)>, "No random accessor.");
		static_assert(!hasRandomAccessor<const decltype(subView)>, "No const random accessor.");
	}
	{
		bzd::test::Range<bzd::typeTraits::IteratorCategory::output> range;
		bzd::range::SubRange subView{range.begin(), range.end()};
		static_assert(!bzd::concepts::sizedRange<decltype(subView)>, "No size member.");
		static_assert(!hasSize<decltype(subView)>, "No size member.");
		static_assert(!hasData<decltype(subView)>, "No data member.");
		static_assert(!hasData<const decltype(subView)>, "No const data member.");
		static_assert(!hasRandomAccessor<decltype(subView)>, "No random accessor.");
		static_assert(!hasRandomAccessor<const decltype(subView)>, "No const random accessor.");
	}
	{
		bzd::test::Range<bzd::typeTraits::IteratorCategory::forward> range;
		bzd::range::SubRange subView{range.begin(), range.end()};
		static_assert(bzd::concepts::sizedRange<decltype(subView)>, "Size member.");
		static_assert(hasSize<decltype(subView)>, "Has size member.");
		static_assert(!hasData<decltype(subView)>, "No data member.");
		static_assert(!hasData<const decltype(subView)>, "No const data member.");
		static_assert(!hasRandomAccessor<decltype(subView)>, "No random accessor.");
		static_assert(!hasRandomAccessor<const decltype(subView)>, "No const random accessor.");
	}
	{
		bzd::test::Range<bzd::typeTraits::IteratorCategory::bidirectional> range;
		bzd::range::SubRange subView{range.begin(), range.end()};
		static_assert(bzd::concepts::sizedRange<decltype(subView)>, "Size member.");
		static_assert(hasSize<decltype(subView)>, "Has size member.");
		static_assert(!hasData<decltype(subView)>, "No data member.");
		static_assert(!hasData<const decltype(subView)>, "No const data member.");
		static_assert(!hasRandomAccessor<decltype(subView)>, "No random accessor.");
		static_assert(!hasRandomAccessor<const decltype(subView)>, "No const random accessor.");
	}
	{
		bzd::test::Range<bzd::typeTraits::IteratorCategory::randomAccess> range;
		bzd::range::SubRange subView{range.begin(), range.end()};
		static_assert(bzd::concepts::sizedRange<decltype(subView)>, "Size member.");
		static_assert(hasSize<decltype(subView)>, "Has size member.");
		static_assert(!hasData<decltype(subView)>, "No data member.");
		static_assert(!hasData<const decltype(subView)>, "No const data member.");
		static_assert(hasRandomAccessor<decltype(subView)>, "No random accessor.");
		static_assert(hasRandomAccessor<const decltype(subView)>, "No const random accessor.");
	}
	{
		bzd::test::Range<bzd::typeTraits::IteratorCategory::contiguous> range;
		bzd::range::SubRange subView{range.begin(), range.end()};
		static_assert(bzd::concepts::sizedRange<decltype(subView)>, "Size member.");
		static_assert(hasSize<decltype(subView)>, "Has size member.");
		static_assert(hasData<decltype(subView)>, "No data member.");
		static_assert(hasData<const decltype(subView)>, "No const data member.");
		static_assert(hasRandomAccessor<decltype(subView)>, "No random accessor.");
		static_assert(hasRandomAccessor<const decltype(subView)>, "No const random accessor.");
	}
}
