#pragma once

#include <string>
#include <variant>
#include <deque>

#include "precedence.h"
#include "scanner.h"

using expr_value = std::variant<bool, int, long, unsigned long long, float, double>;

class parser
{
public:
	parser(std::string file_name);
	bool parse(expr_value& value);
	bool parse_expression(expr_value& value);
	bool parse_binary_expression(expr_value& value);
	bool parse_binary_expression_prime(expr_value& value, operator_precedence minimal_precedence);
	bool parse_unary_expression(expr_value& value);
private:
	std::string _source;
	scanner _scanner;
	std::deque<token> _cache_tokens;
	token _lookahead_token{ token::END_OF_FILE };
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
};

