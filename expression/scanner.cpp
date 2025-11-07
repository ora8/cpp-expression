#include "scanner.h"

token scanner::next()
{
	while (_source_iter != _source.end() && std::isspace(*_source_iter))
	{
		if (*(_source_iter++) == '\0')
			_line++;
	}
	_column = _source_iter - _source.begin() + 1;
	if (_source_iter == _source.end())
		return { token_kind::END_OF_FILE, _line, _column, 0 };
	switch (*(_source_iter++))
	{
	case '*':
		return { token_kind::STAR, _line, _column, 0 };
	case '/':
		return { token_kind::SLASH, _line, _column, 0 };
	case '%':
		return { token_kind::PERCENT, _line, _column, 0 };
	case '+':
		if (_source_iter != _source.end() && *_source_iter >= '0' && *_source_iter <= '9')
		{
			return scan_number_literal();
		}
		return { token_kind::PLUS, _line, _column, 0 };
	case '-':
		if (_source_iter != _source.end() && *_source_iter >= '0' && *_source_iter <= '9')
		{
			return scan_number_literal();
		}
		return { token_kind::DASH, _line, _column, 0 };
	case '<':
		if (_source_iter != _source.end() && *_source_iter == '<')
		{
			++_source_iter;
			return { token_kind::LESS_LESS, _line, _column, 0 };
		}
		return { token_kind::LESS, _line, _column, 0
		};
	case '>':
		if (_source_iter != _source.end())
		{
			if (*_source_iter == '>')
			{
				++_source_iter;
				return { token_kind::GREATER_GREATER, _line, _column, 0 };
			}
			else if (*_source_iter == '=')
			{
				++_source_iter;
				return { token_kind::GREATER_EQUAL, _line, _column, 0 };
			}
		}
		return { token_kind::GREATER, _line, _column, 0 };
	case '=':
		if (_source_iter < _source.end() && *_source_iter == '=')
		{
			++_source_iter;
			return { token_kind::EQUAL_EQUAL, _line, _column, 0 };
		}
		return { token_kind::EQUAL, _line, _column, 0 };
	case '!':
		if (_source_iter != _source.end() && *_source_iter == '=')
		{
			++_source_iter;
			return { token_kind::EXCLAIM_EQUAL, _line, _column, 0 };
		}
		return { token_kind::NOT, _line, _column, 0 };
	case '&':
		if (_source_iter != _source.end() && *_source_iter == '&')
		{
			++_source_iter;
			return { token_kind::AMP_AMP, _line, _column, 0 };
		}
		return {token_kind::AMP, _line, _column, 0 };
	case '^':
		return { token_kind::CARET, _line, _column, 0 };
	case '|':
		if (_source_iter != _source.end() && *_source_iter == '|')
		{
			++_source_iter;
			return { token_kind::BAR_BAR, _line, _column, 0 };
		}
		return { token_kind::BAR, _line, _column, 0 };
	case '(':
		return { token_kind::LPAREN, _line, _column, 0 };
	case ')':
		return { token_kind::RPAREN, _line, _column, 0};
	default:
		auto ch = *(--_source_iter);
		if (isdigit(ch))
		{
			return scan_number_literal();
		}
		if (ch == '"')
		{
			std::string value;
			while (_source_iter++ != _source.end() && *_source_iter != '"')
			{
				value += ch;
			}
			if (*_source_iter == '"')
				_source_iter++;
			return { token_kind::STRING_LITERAL, _line, _column, 0, value };
		}
		return { token_kind::INVALID_CHARACTER, _line, _column, 0 };
	}
}

token scanner::scan_number_literal()
{
	std::string value;
	auto negative{ *_source_iter == '-' };
	if (*_source_iter == '+' || negative)
		_source_iter++;
	char ch = *_source_iter;
	auto is_float{ false };
	value += ch;
	++_source_iter;
	while (_source_iter != _source.end())
	{
		ch = *_source_iter;
		if (ch == '.' && !is_float)
			is_float = true;
		else if (ch < '0' || ch > '9')
			break;
		value += ch;
		++_source_iter;
	}
	if (is_float)
	{
		if (_source_iter != _source.end() && (*_source_iter == 'e' || *_source_iter == 'E'))
			value += *(_source_iter++);
		if (_source_iter != _source.end() && (*_source_iter == '+' || *_source_iter == '-'))
			value += *(_source_iter++);
		while (_source_iter != _source.end())
		{
			ch = *_source_iter;
			if (ch < '0' || ch > '9')
				break;
			value += ch;
			++_source_iter;
		}

		auto d = std::stod(value);
		if (d < std::numeric_limits<float>::min() || d > std::numeric_limits<float>::max())
			return { token_kind::DOUBLE_LITERAL, _line, _column, 0 };
		return { token_kind::FLOAT_LITERAL, _line, _column, static_cast<float>(d) };
	}
	if (negative)
	{
		auto ll = std::stoll(value);
		if (ll < std::numeric_limits<long>::min() || ll > std::numeric_limits<long>::max())
			return { token_kind::LONG_LONG_LITERAL, _line, _column, ll };
		if (ll < std::numeric_limits<int>::min() || ll > std::numeric_limits<int>::max())
			return { token_kind::LONG_LITERAL, _line, _column, static_cast<long>(ll) };
		return { token_kind::INT_LITERAL, _line, _column, static_cast<int>(ll) };

	}
	auto ull = std::stoull(value);
	if (ull > std::numeric_limits<unsigned long>::max())
		return { token_kind::UNSIGNED_LONG_LONG_LITERAL, _line, _column, ull };
	if (ull > std::numeric_limits<unsigned int>::max())
		return { token_kind::UNSIGNED_LONG_LITERAL, _line, _column, static_cast<unsigned int>(ull) };
	return { token_kind::UNSIGNED_INT_LITERAL, _line, _column, static_cast<unsigned int>(ull) };
}