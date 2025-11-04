#pragma once

#include "scanner.h"

enum class operator_precedence : size_t
{
	NO_PRECEDENCE,
	LOGICAL_OR,
	LOGICAL_AND,
	INCLUSIVE_OR,
	EXCLUSIVE_OR,
	AND_,
	EQUALITY,
	RELATIONAL,
	SHIFT,
	STROKE,
	POINT,
};

class precedence
{
public:
	precedence() = default;
	bool is_binary_operator(token tok, operator_precedence &precedence);
	static precedence get_instance()
	{
		static precedence instance;
		return instance;
	}
private:
};
