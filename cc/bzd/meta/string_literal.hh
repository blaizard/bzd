#pragma once

#include "cc/bzd/meta/range.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::meta {

/// Type to be used as a template argument.
template <bzd::SizeType n>
class StringLiteral
{
public:
	constexpr StringLiteral(char const (&arr)[n]) noexcept : StringLiteral(arr, bzd::meta::Range<0, n>{}) {}

public:
	[[nodiscard]] constexpr const char* data() const noexcept { return data_; }
	[[nodiscard]] constexpr char* data() noexcept { return data_; }
	[[nodiscard]] static constexpr bzd::SizeType size() noexcept { return n; }

private:
	template <bzd::SizeType... i>
	constexpr StringLiteral(const char (&arr)[n], bzd::meta::range::Type<i...>) noexcept : data_{arr[i]..., '\0'}
	{
	}

public: // Must be public to be structural (requirement for template literals).
	const char data_[n + 1];
};

} // namespace bzd::meta
