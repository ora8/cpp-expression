#pragma once

#include <string>
#include <variant>
#include <deque>

#include "precedence.h"
#include "scanner.h"
#include "token.h"

class parser
{
public:
	parser(const std::string& filename);
	bool parse(token_value& value);
private:
	std::string _source;
	scanner _scanner;
	unsigned int _error_distance{ 3 };
	unsigned int _errors{ 0 };
	size_t _line;
	token _lookahead_token;
	token _token;
	size_t _pos;
	bool check(token_kind expected_token_kind, const std::string& error_message);
	void error(const std::string& message);
	void error(std::string&& message);
	bool is_primary_expression(token token)
	{
		switch (token.kind)
		{
		case token_kind::INT_LITERAL:
		case token_kind::UNSIGNED_INT_LITERAL:
		case token_kind::LONG_LITERAL:
		case token_kind::UNSIGNED_LONG_LITERAL:
		case token_kind::LONG_LONG_LITERAL:
		case token_kind::UNSIGNED_LONG_LONG_LITERAL:
		case token_kind::FLOAT_LITERAL:
		case token_kind::DOUBLE_LITERAL:
			return true;
		default:
			return false;
		}
	}
	void scan(size_t n = 0)
	{
		_token = _lookahead_token;
		_pos = _scanner.column();
		_line = _scanner.line();
		_lookahead_token = _scanner.next();
		_error_distance++;
	}
	bool parse_expression(token_value& value);
	bool parse_binary_expression(token_value& value);
	bool parse_binary_expression_prime(token_value& value, operator_precedence minimal_precedence);
	bool parse_hex_literal(token_value& value, bool& is_hex);
	bool parse_integer_literal(token_value& value);
	bool parse_primary_expression(token_value& value);
	bool parse_unary_expression(token_value& value);

	token_value add(token_value& lhs, token_value& rhs);
	token_value subtract(token_value& lhs, token_value& rhs);
	token_value multiply(token_value& lhs, token_value& rhs);
	token_value divide(token_value& lhs, token_value& rhs);
	token_value modulus(token_value& lhs, token_value& rhs);
	token_value left_shift(token_value& left, token_value& right);
	token_value right_shift(token_value& left, token_value& right);
	token_value bitwise_and(token_value& left, token_value& right);
	token_value bitwise_not(token_value& value);
	token_value bitwise_or(token_value& left, token_value& right);
	bool greater(token_value& left, token_value& right);
	bool less(token_value& left, token_value& right);
	bool greater_equal(token_value& left, token_value& right);
	bool less_equal(token_value& left, token_value& right);
	bool logical_and(token_value& left, token_value& right);
	bool logical_or(token_value& left, token_value& right);
	token_value bitwise_exclusive_or(token_value& left, token_value& right);
	token_value negate(token_value& value);
	token_value not_(token_value& value);
};

