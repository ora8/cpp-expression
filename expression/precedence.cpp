#include <cstring>

#include "precedence.h"
#include "scanner.h"

static operator_precedence precedence_table[static_cast<size_t>(token_kind::BAR_BAR) + 1] =
{
	operator_precedence::NO_PRECEDENCE, // token::UNDEFINED
	operator_precedence::POINT,        // token::STAR
	operator_precedence::POINT,        // token::SLASH
	operator_precedence::POINT,        // token::PERCENT
	operator_precedence::STROKE,       // token::ADD
	operator_precedence::STROKE,       // token::SUB
	operator_precedence::SHIFT,        // token::LESS_LESS
	operator_precedence::SHIFT,        // token::GREATER_GREATER
	operator_precedence::RELATIONAL,   // token::LESS
	operator_precedence::RELATIONAL,   // token::LESS_EQUAL
	operator_precedence::RELATIONAL,   // token::GREATER
	operator_precedence::RELATIONAL,   // token::GREATER_EQUAL
	operator_precedence::NO_PRECEDENCE, // token::EQUAL
	operator_precedence::EQUALITY,     // token::EQUAL_EQUAL
	operator_precedence::EQUALITY,     // token::EXCLAIM_EQUAL
	operator_precedence::AND_,         // token::AMP
	operator_precedence::EXCLUSIVE_OR, // token::CARET
	operator_precedence::INCLUSIVE_OR, // token::BAR
	operator_precedence::LOGICAL_AND,  // token::AMP_AMP
	operator_precedence::LOGICAL_OR,   // token::BAR_BAR
};

bool precedence::is_binary_operator(token token, operator_precedence& precedence)
{
	if (static_cast<size_t>(token.kind) >= sizeof(precedence_table) / sizeof(precedence_table[0]) ||
		token.kind == token_kind::UNDEFINED)
	{
		return false;
	}
	precedence = precedence_table[static_cast<size_t>(token.kind)];
	return true;
}