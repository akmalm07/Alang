#pragma once


#include "tokens.h"

#include <vector>
#include <string_view>

namespace alang {

	std::vector<Token> tokenize(const std::string& source);

}