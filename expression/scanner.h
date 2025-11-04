#pragma once

#include <iostream>
#include <string>
#include <vector>

enum class token : size_t
{
	UNDEFINED,
	STAR,
	SLASH,
	PERCENT,
	PLUS,
	MINUS,
	LESS_LESS,
	GREATER_GREATER,
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	EQUAL,
	EQUAL_EQUAL,
	EXCLAIM_EQUAL,
	AMP,
	CARET,
	BAR,
	AMP_AMP,
	BAR_BAR,
	TILDE,
	EXCLAIM,
	LPAREN,
	RPAREN,
	IDENTIFIER,
	NUMBER ,
	ALIGNAS,
	ALIGNOF,
	ASM,
	AUTO,
	BITAND,
	BITOR,
	BOOL,
	BREAK,
	CASE,
	CATCH,
	CHAR,
	CHAR16_T,
	CHAR32_T,
	CLASS,
	COMPL,
	CONST,
	CONSTEVAL,
	CONSTEXPR,
	CONST_CAST,
	CONTINUE,
	DECLTYPE,
	DEFAULT,
	DELETE,
	DO,
	DOUBLE,
	DYNAMIC_CAST,
	ELSE,
	ENUM,
	EXPLICIT,
	EXPORT,
	EXTERN,
	FALSE,
	FLOAT,
	FOR,
	FRIEND,
	GOTO,
	IF,
	INLINE,
	INT,
	LONG,
	MUTABLE,
	NAMESPACE,
	NEW,
	NOEXCEPT,
	NOT,
	NOT_EQ,
	NULLPTR,
	OPERATOR,
	OR,
	OR_EQ,
	PRIVATE,
	PROTECTED,
	PUBLIC,
	REGISTER,
	REINTERPRET_CAST,
	REQUIRES,
	RETURN,
	SHORT,
	SIGNED,
	SIZEOF,
	STATIC,
	STATIC_ASSERT,
	STATIC_CAST,
	STRUCT,
	SWITCH,
	SYNCHRONIZED,
	TEMPLATE,
	THIS,
	THROW,
	THROWS,
	TRUE,
	TRY,
	TYPEDEF,
	TYPEID,
	TYPENAME,
	UNION,
	UNSIGNED,
	USING,
	VIRTUAL,
	VOID,
	VOLATILE,
	WCHAR_T,
	WHILE,
	XOR,
	XOR_EQ,	
	END_OF_FILE,
};

class scanner
{
public:
	scanner() = default;
	void error(const std::string& message)
	{
		std::cerr << "Line " << _line << ", " << "pos " << _location << ": " << message << std::endl;
	}
	void error(std::string&& message)
	{
		std::cerr << "Line " << _line << ", " << "pos " << _location << ": " << message << std::endl;
	}	
	token scan();
	std::string identifier() const
	{
		return _identifier;
	}
	size_t location() const
	{
		return _location;
	}	
	int number() const
	{
		return _number;
	}
	void set_source(const std::string& source)
	{
		_source = source;
		_pos = 0;
		_line = 1;
	}
private:
	std::string _source;
	size_t _location{ 1 };
	size_t _pos{ 0 };
	size_t _line{ 1 };
	token _token{ token::END_OF_FILE };
	std::string _identifier;
	int _number{ 0 };
};

