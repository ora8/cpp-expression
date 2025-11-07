#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "token.h"

class parser;

class scanner
{
public:
	scanner(const parser& parser) : _parser{ parser } 
	{}
	token next();
	std::string identifier() const
	{
		return _identifier;
	}
	size_t line() const
	{
		return _line;
	}
	size_t column() const
	{
		return _column;
	}	
	std::string value() const
	{
		return _value;
	}
	void set_source(const std::string& source)
	{
		_source = source;
		_source_iter = _source.begin();;
		_line = 1;
	}
private:
	const parser& _parser;
	std::string _source;
	size_t _column{ 1 };
	std::string::iterator _source_iter;
	size_t _line{ 1 };
	token _token{ token_kind::END_OF_FILE };
	std::string _identifier;
	std::string _value;
	token scan_number_literal();
};

