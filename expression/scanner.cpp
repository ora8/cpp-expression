#include "scanner.h"

token scanner::scan()
{
	while (_source_iter != _source.end() && std::isspace(*_source_iter))
	{
		if (*(_source_iter++) == '\0')
			_line++;
	}
	_location = _source_iter - _source.begin() + 1;
	if (_source_iter == _source.end())
		return token::END_OF_FILE;
	switch (*(_source_iter++))
	{
	case '*':
		return token::STAR;
	case '/':
		return token::SLASH;
	case '%':
		return token::PERCENT;
	case '+':
		if (_source_iter != _source.end() && *_source_iter >= '0' && *_source_iter <= '9')
		{
			return scan_number_literal(false);
		}
		return token::PLUS;
	case '-':
		if (_source_iter != _source.end() && *_source_iter >= '0' && *_source_iter <= '9')
		{
			return scan_number_literal(true);
		}
		return token::MINUS;
	case '<':
		if (_source_iter != _source.end() && *_source_iter == '<')
		{
			++_source_iter;
			return token::LESS_LESS;
		}
		return token::LESS;
	case '>':
		if (_source_iter != _source.end())
		{
			if (*_source_iter == '>')
			{
				++_source_iter;
				return token::GREATER_GREATER;
			}
			else if (*_source_iter == '=')
			{
				++_source_iter;
				return token::GREATER_EQUAL;
			}
		}
		return token::GREATER;
	case '=':
		if (_source_iter < _source.end() && *_source_iter == '=')
		{
			++_source_iter;
			return token::EQUAL_EQUAL;
		}
		return token::EQUAL;
	case '!':
		if (_source_iter != _source.end() && *_source_iter == '=')
		{
			++_source_iter;
			return token::EXCLAIM_EQUAL;
		}
		return token::NOT;
	case '&':
		if (_source_iter != _source.end() && *_source_iter == '&')
		{
			++_source_iter;
			return token::AMP_AMP;
		}
		return token::AMP;
	case '^':
		return token::CARET;
	case '|':
		if (_source_iter != _source.end() && *_source_iter == '|')
		{
			++_source_iter;
			return token::BAR_BAR;
		}
		return token::BAR;
	case '(':
		return token::LPAREN;
	case ')':
		return token::RPAREN;
	default:
		auto ch = *(--_source_iter);
		if (isdigit(ch))
		{
			return scan_number_literal(false);
		}
		if (ch == '"')
		{
			_value.clear();
			while (_source_iter++ != _source.end() && *_source_iter != '"')
			{
				_value += ch;
			}
			if (*_source_iter == '"')
				_source_iter++;
		}
		return token::INVALID_CHARACTER;
	}
}

token scanner::scan_number_literal(bool is_neg)
{
	if (is_neg)
		_value = '-';
	else
		_value.clear();
	char ch = *_source_iter;
	if (ch < '0' || ch > '9')
		return token::UNDEFINED;

	auto is_float{ false };
	_value += ch;
	++_source_iter;
	while (_source_iter != _source.end())
	{
		ch = *_source_iter;
		if (ch == '.' && !is_float)
			is_float = true;
		else if (ch < '0' || ch > '9')
			break;
		_value += ch;
		++_source_iter;
	}
	if (is_float)
	{
		if (_source_iter != _source.end() && (*_source_iter == 'e' || *_source_iter == 'E'))
			_value += *(_source_iter++);
		if (_source_iter != _source.end() && (*_source_iter == '+' || *_source_iter == '-'))
			_value += *(_source_iter++);
		while (_source_iter != _source.end())
		{
			ch = *_source_iter;
			if (ch < '0' || ch > '9')
				break;
			_value += ch;
			++_source_iter;
		}
		return token::FLOAT_LITERAL;
	}
	return token::INTEGER_LITERAL;
 }