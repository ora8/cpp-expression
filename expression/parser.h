#pragma once

#include <string>
#include <variant>
#include <deque>

#include "precedence.h"
#include "scanner.h"
#include "token.h"

using expr_value = std::variant<bool, char, unsigned char, short, unsigned short, int, unsigned int,  
		long, unsigned long, long long, unsigned long long, float, double>;

class parser
{
public:
	parser(std::string file_name);
	bool parse(expr_value& value);
private:
	std::string _source;
	scanner _scanner;
	std::deque<token> _cache_tokens;
	token _lookahead_token{ token::END_OF_FILE };
	void error(const std::string& message);
	void error(std::string&& message);
	void insert_token(token token)
	{
		_cache_tokens.push_front(token);
		_lookahead_token = token;
	}
	void consume_token()
	{
		if (!_cache_tokens.empty())
		{
			_lookahead_token = _cache_tokens.front();
			_cache_tokens.pop_front();
		}
		if (!_cache_tokens.empty())
			_lookahead_token = _cache_tokens.front();
		else
			_lookahead_token = _scanner.scan();
	}
	bool is_primary_expression(token token)
	{
		switch (token)
		{
		case token::INTEGER_LITERAL:
		case token::FLOAT_LITERAL:
			return true;
		default:
			return false;
		}
	}
	token lookahead_token(size_t n = 0)
	{
		if (n == 0)
			return _lookahead_token;
		auto m = n - _cache_tokens.size();
		while (m-- > 0)
		{
			_cache_tokens.push_back(_scanner.scan());
		}
		return _cache_tokens[n];
	}
	bool parse_expression(expr_value& value);
	bool parse_binary_expression(expr_value& value);
	bool parse_binary_expression_prime(expr_value& value, operator_precedence minimal_precedence);
	bool parse_hex_literal(expr_value& value, bool& is_hex);
	bool parse_integer_literal(expr_value& value);
	bool parse_primary_expression(expr_value& value);
	bool parse_unary_expression(expr_value& value);

	expr_value add(expr_value& lhs, expr_value& rhs);
	expr_value subtract(expr_value& lhs, expr_value& rhs);
	expr_value multiply(expr_value& lhs, expr_value& rhs);
	expr_value divide(expr_value& lhs, expr_value& rhs);
	expr_value modulus(expr_value& lhs, expr_value& rhs);
	expr_value left_shift(expr_value& left, expr_value& right);
	expr_value right_shift(expr_value& left, expr_value& right);
	expr_value bitwise_and(expr_value& left, expr_value& right);
	expr_value bitwise_not(expr_value& value);
	expr_value bitwise_or(expr_value& left, expr_value& right);
	bool greater(expr_value& left, expr_value& right);
	bool less(expr_value& left, expr_value& right);
	bool greater_equal(expr_value& left, expr_value& right);
	bool less_equal(expr_value& left, expr_value& right);
	bool logical_and(expr_value& left, expr_value& right);
	bool logical_or(expr_value& left, expr_value& right);
	expr_value bitwise_exclusive_or(expr_value& left, expr_value& right);
	expr_value negate(expr_value& value);
	expr_value not_(expr_value& value);
};

