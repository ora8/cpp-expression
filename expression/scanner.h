#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "token.h"

class scanner
{
public:
	scanner() = default;
	token scan();
	std::string identifier() const
	{
		return _identifier;
	}
	size_t line() const
	{
		return _line;
	}
	size_t location() const
	{
		return _location;
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
	std::string _source;
	size_t _location{ 1 };
	std::string::iterator _source_iter;
	size_t _line{ 1 };
	token _token{ token::END_OF_FILE };
	std::string _identifier;
	std::string _value;
	token scan_number_literal(bool is_neg);
};

