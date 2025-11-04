#include "scanner.h"

token scanner::scan()
{
	_location = _pos + 1;
	while (_pos < _source.size() && std::isspace(_source[_pos]))
	{
		if (_source[_pos++] == '\0')
			_line++;
	}

	if (_pos == _source.size())
		return token::END_OF_FILE;
	switch (_source[_pos])
	{
	case '*':
		++_pos;
		return token::STAR;
	case '/':
		++_pos;
		return token::SLASH;
	case '%':
		++_pos;
		return token::PERCENT;
	case '+':
		++_pos;
		return token::PLUS;
	case '-':
		++_pos;
		return token::MINUS;
	case '<':
		++_pos;
		if (_pos < _source.size())
		{
			if (_source[_pos] == '<')
			{
				++_pos;
				return token::LESS_LESS;
			}
			else if (_source[_pos] == '=')
			{
				++_pos;
				return token::LESS_EQUAL;
			}
		}
		return token::LESS;
	case '>':
		++_pos;
		if (_pos < _source.size())
		{
			if (_source[_pos] == '>')
			{
				++_pos;
				return token::GREATER_GREATER;
			}
			else if (_source[_pos] == '=')
			{
				++_pos;
				return token::GREATER_EQUAL;
			}
		}
		return token::GREATER;
	case '=':
		++_pos;
		if (_pos < _source.size() && _source[_pos] == '=')
		{
			++_pos;
			return token::EQUAL_EQUAL;
		}
		return token::EQUAL;
	case '!':
		++_pos;
		if (_pos < _source.size())
		{
			if (_source[_pos] == '=')
			{
				++_pos;
				return token::EXCLAIM_EQUAL;
			}
			else
			{
				error("= execpected");
				return scan();
			}
		}
	case '&':
		++_pos;
		if (_pos < _source.size() && _source[_pos] == '&')
		{
			++_pos;
			return token::AMP_AMP;
		}
		return token::AMP;
	case '^':
		++_pos;
		return token::CARET;
	case '|':
		++_pos;
		if (_pos < _source.size() && _source[_pos] == '|')
		{
			++_pos;
			return token::BAR_BAR;
		}
		return token::BAR;
	case '(':
		++_pos;
		return token::LPAREN;
	case ')':
		++_pos;
		return token::RPAREN;
	default:
		auto ch = _source[_pos];
		if (std::isalpha(ch) || ch == '_')
		{
			_identifier = ch;
			++_pos;
			while (_pos < _source.size())
			{
				ch = _source[_pos];
				if (!(std::isalnum(ch) || ch == '_'))
					break;
				_identifier += ch;
				++_pos;
			}
			return token::IDENTIFIER;
		}
		else if (ch >= '0' && ch <= '9')
		{
			_number = ch - '0';
			++_pos;
			while (_pos < _source.size())
			{
				ch = _source[_pos];
				if (ch < '0' || ch > '9')
					break;
				_number = 10 * _number + ch - '0';
				++_pos;
			}
			return token::NUMBER;
		}
		else
		{
			error(std::string("unexpected character ") + ch);
			++_pos;
			return scan();
		}
	}
}
