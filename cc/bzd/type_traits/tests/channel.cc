#include "cc/bzd/type_traits/channel.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/ichannel_buffered.hh"
#include "cc/bzd/test/test.hh"

TEST(TypeTraits, ReaderAsync)
{
	static_assert(bzd::concepts::readerAsync<bzd::IChannelBuffered<char, 16u>>);
	static_assert(!bzd::concepts::readerAsync<bzd::Array<char, 16u>>);
}
