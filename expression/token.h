#pragma once

#include <variant>

enum class token_kind : unsigned int
{
	UNDEFINED = 1,
	STAR = 2,
	SLASH = 3,
	PERCENT = 4,
	PLUS = 5,
	DASH = 6,
	LESS_LESS = 7,
	GREATER_GREATER = 8,
	LESS = 9,
	LESS_EQUAL = 10,
	GREATER = 11,
	GREATER_EQUAL = 12,
	EQUAL = 13,
	EQUAL_EQUAL = 14,
	EXCLAIM_EQUAL = 15,
	AMP = 16,
	CARET = 17,
	BAR = 18,
	AMP_AMP = 19,
	BAR_BAR = 20,
	TILDE = 21,
	EXCLAIM = 22,
	LPAREN = 23,
	RPAREN = 24,
	AND = 25,
	AND_EQ = 26,
	BITAND = 27,
	BITOR = 28,
	COMPL = 29,
	NOT = 30,
	NOT_EQ = 31,
	OR = 32,
	OR_EQ = 33,
	XOR = 34,
	XOR_EQ = 35,
	INT_LITERAL = 100,
	UNSIGNED_INT_LITERAL = 102,
	LONG_LITERAL = 103,
	UNSIGNED_LONG_LITERAL = 104,
	LONG_LONG_LITERAL = 105,
	UNSIGNED_LONG_LONG_LITERAL = 106,
	FLOAT_LITERAL = 107,
	DOUBLE_LITERAL = 108,
	STRING_LITERAL = 109,
	INVALID_CHARACTER = 200,
	END_OF_FILE = 300,
};

using token_value = std::variant<bool, char, unsigned char, short, unsigned short, int, unsigned int,
	long, unsigned long, long long, unsigned long long, float, double>;

struct token 
{
	token_kind kind;
	size_t    line;	  // token line (starts at 1)
	size_t    column;	  // token column (starts at 1)
	token_value value;
	std::string str;
};