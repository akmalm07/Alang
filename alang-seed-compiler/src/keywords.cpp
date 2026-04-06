#include "headers.h"
#include "keywords.h"


namespace alang {

	std::optional<Keyword> string_to_keyword(std::string_view str)
	{
		for (size_t i = 0; i < c_keyword_count; i++)
		{
			if (str == c_keywords[i])
				return static_cast<Keyword>(i);
		}
		return std::nullopt;
	}

	constexpr std::string_view keyword_to_string(Keyword keyword)
	{
		size_t index = static_cast<size_t>(keyword);
		if (index < c_keyword_count)
			return c_keywords[index];
		return "<invalid keyword>";
	}


}