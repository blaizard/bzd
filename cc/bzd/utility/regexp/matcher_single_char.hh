#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/utility/regexp/types.hh"

namespace bzd::regexp {

class MatcherSingleChar
{
private:
	using MemberPointer = Result (MatcherSingleChar::*)(const char);
	constexpr explicit MatcherSingleChar(const MemberPointer member, const char c = '\0') noexcept : member_{member}, c_{c} {}

public:
	static constexpr bzd::Optional<MatcherSingleChar> make(bzd::StringView& regexp) noexcept
	{
		auto value = regexp[0];
		if (value == '*' || value == '+' || value == '?')
		{
			return bzd::nullopt;
		}
		else if (value == '.')
		{
			regexp.removePrefix(1u);
			return MatcherSingleChar{&MatcherSingleChar::matchAll};
		}
		else if (value == '\\')
		{
			if (regexp.size() < 2u)
			{
				return bzd::nullopt;
			}
			value = regexp[1];
			regexp.removePrefix(2u);
			switch (value)
			{
			case 's':
				return MatcherSingleChar{&MatcherSingleChar::matchSpace};
			case 'S':
				return MatcherSingleChar{&MatcherSingleChar::matchNotSpace};
			case 'w':
				return MatcherSingleChar{&MatcherSingleChar::matchWord};
			case 'W':
				return MatcherSingleChar{&MatcherSingleChar::matchNotWord};
			}
		}
		else
		{
			regexp.removePrefix(1u);
		}
		return MatcherSingleChar{&MatcherSingleChar::matchChar, value};
	}

	constexpr Result operator()(const char c) noexcept { return (this->*member_)(c); }

private:
	constexpr Result matchAll(const char) noexcept { return Success::last; }

	constexpr Result matchChar(const char c) noexcept
	{
		if (c == c_)
		{
			return Success::last;
		}
		return bzd::error::make(Error::noMatch);
	}

	constexpr Result matchWord(const char c) noexcept
	{
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c >= '_')
		{
			return Success::last;
		}
		return bzd::error::make(Error::noMatch);
	}

	constexpr Result matchNotWord(const char c) noexcept
	{
		if (!matchWord(c))
		{
			return Success::last;
		}
		return bzd::error::make(Error::noMatch);
	}

	constexpr Result matchSpace(const char c) noexcept
	{
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f')
		{
			return Success::last;
		}
		return bzd::error::make(Error::noMatch);
	}

	constexpr Result matchNotSpace(const char c) noexcept
	{
		if (!matchSpace(c))
		{
			return Success::last;
		}
		return bzd::error::make(Error::noMatch);
	}

private:
	MemberPointer member_;
	char c_;
};

} // namespace bzd::regexp
