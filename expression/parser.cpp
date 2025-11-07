#include <fstream>
#include <iostream>
#include <sstream>

#include "parser.h"
#include "precedence.h"
#include "scanner.h"

token_value parser::add(token_value& lhs, token_value& rhs) 
{
	return std::visit([](auto&& a, auto&& b) -> token_value 
	{
		return a + b;
	}, lhs, rhs);
}

token_value parser::subtract(token_value& lhs, token_value& rhs) 
{
	return std::visit([](auto&& a, auto&& b) -> token_value 
	{
		return a - b;
	}, lhs, rhs);
}

token_value parser::multiply(token_value& lhs, token_value& rhs) 
{
	return std::visit([](auto&& a, auto&& b) -> token_value 
	{
		return a * b;
	}, lhs, rhs);
}

token_value parser::divide(token_value& lhs, token_value& rhs) 
{
	return std::visit([this](auto&& a, auto&& b) -> token_value 
		{
		using T1 = std::decay_t<decltype(a)>;
		using T2 = std::decay_t<decltype(b)>;
		// Check if both types are compatible for modulus
		if constexpr (std::is_arithmetic_v<T1> && !std::is_same_v<T1, bool> &&
			std::is_arithmetic_v<T2> && !std::is_same_v<T2, bool>) {
			return a / b; // Perform modulus operation
		}
		else {
			error("Divide operation is only defined for int, long, unsigned long long, flaot and double.");
			return 0; // Return a default value to satisfy the return type
		}
		}, lhs, rhs);
}

token_value parser::modulus(token_value& lhs, token_value& rhs) 
{
	return std::visit([this](auto&& a, auto&& b) -> token_value 
	{
		using T1 = std::decay_t<decltype(a)>;
		using T2 = std::decay_t<decltype(b)>;

		// Check if both types are compatible for modulus
		if constexpr ((std::is_same_v<T1, int> || std::is_same_v<T1, long> || std::is_same_v<T1, unsigned long long>) &&
			(std::is_same_v<T2, int> || std::is_same_v<T2, long> || std::is_same_v<T2, unsigned long long>)) {
			return a % b; // Perform modulus operation
		}
		else {
			error("Modulus operation is only defined for int, long, and unsigned long long.");
			return 0; // Return a default value to satisfy the return type
		}
		}, lhs, rhs);
}

token_value parser::left_shift(token_value& left, token_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> token_value 
	{
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both lhs and rhs are integral types and rhs is non-negative and not boolean
		if constexpr (std::is_integral_v<T1> && !std::is_same_v<T1, bool> &&
			std::is_integral_v<T2> && !std::is_same_v<T2, bool>) 
		{
			if (b < 0) 
			{
				error("Right-hand side must be non-negative for left shift.");
				return a; // Return the original value or handle as needed
			}
			return a << b; // Perform left bitwise shift
		}
		else 
		{
			error("Left shift can only be applied to integral types.");
			return a; // Return the original value or handle as needed
		}
	}, left, right);
}

token_value parser::right_shift(token_value& left, token_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> token_value 
	{
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure rhs is an integral type and non-negative
		if constexpr (std::is_integral_v<T1> && !std::is_same_v<T1, bool> &&
			std::is_integral_v<T2> && !std::is_same_v<T2, bool>) 
		{
			if (b < 0) 
			{
				error("Right-hand side must be non-negative for right shift.");
				return a;
			}
			return a >> b; // Perform right bitwise shift
		}
		else 
		{
			error("Left shift can only be applied to integral types.");
			return a;
		}
	}, left, right);
}

bool parser::less_equal(token_value& left, token_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> bool 
		{
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are arithmetic types and not bool
		if constexpr (std::is_arithmetic_v<T1> && !std::is_same_v<T1, bool> &&
			std::is_arithmetic_v<T2> && !std::is_same_v<T2, bool>) 
		{
			// Handle signed/unsigned comparison
			if constexpr (std::is_signed_v<T1> && std::is_unsigned_v<T2>) 
			{
				// Convert T1 (signed) to the corresponding unsigned type
				return b <= static_cast<T2>(a < 0 ? 0 : a); // Handle negative values
			}
			else if constexpr (std::is_unsigned_v<T1> && std::is_signed_v<T2>) 
			{
				// Convert T2 (signed) to the corresponding unsigned type
				return static_cast<T1>(b < 0 ? 0 : b) <= a; // Handle negative values
			}
			else 
			{
				return a <= b; // Perform greater than or equal comparison
			}
		}
		else 
		{
			error("Less than or equal comparison can only be applied to integral types.");
			return false; // Return a default value or handle as needed
		}
		}, left, right);
}

bool parser::greater_equal(token_value& left, token_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> bool 
	{
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are arithmetic types and not bool
		if constexpr (std::is_arithmetic_v<T1> && !std::is_same_v<T1, bool> &&
			std::is_arithmetic_v<T2> && !std::is_same_v<T2, bool>) 
		{
			// Handle signed/unsigned comparison
			if constexpr (std::is_signed_v<T1> && std::is_unsigned_v<T2>) 
			{
				// Convert T1 (signed) to the corresponding unsigned type
				return b >= static_cast<T2>(a < 0 ? 0 : a); // Handle negative values
			}
			else if constexpr (std::is_unsigned_v<T1> && std::is_signed_v<T2>) 
			{
				// Convert T2 (signed) to the corresponding unsigned type
				return static_cast<T1>(b < 0 ? 0 : b) >= a; // Handle negative values
			}
			else 
			{
				return a >= b; // Perform greater than or equal comparison
			}
		}
		else 
		{
			error("Greater than or equal comparison can only be applied to integral types.");
			return false; // Return a default value or handle as needed
		}
	}, left, right);
}

bool parser::less(token_value& left, token_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> bool 
	{
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are arithmetic types and not bool
		if constexpr (std::is_arithmetic_v<T1> && !std::is_same_v<T1, bool> &&
			std::is_arithmetic_v<T2> && !std::is_same_v<T2, bool>) 
		{
			// Handle signed/unsigned comparison
			if constexpr (std::is_signed_v<T1> && std::is_unsigned_v<T2>) 
			{
				// Convert T1 (signed) to the corresponding unsigned type
				return b < static_cast<T2>(a < 0 ? 0 : a); // Handle negative values
			}
			else if constexpr (std::is_unsigned_v<T1> && std::is_signed_v<T2>) 
			{
				// Convert T2 (signed) to the corresponding unsigned type
				return static_cast<T1>(b < 0 ? 0 : b) < a; // Handle negative values
			}
			else 
			{
				return a < b; // Perform greater than or equal comparison
			}
		}
		else 
		{
			error("Less than comparison can only be applied to integral types.");
			return false; // Return a default value or handle as needed
		}
	}, left, right);
}

bool parser::greater(token_value& left, token_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> bool 
	{
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are arithmetic types and not bool
		if constexpr (std::is_arithmetic_v<T1> && !std::is_same_v<T1, bool> &&
			std::is_arithmetic_v<T2> && !std::is_same_v<T2, bool>) 
		{
			// Handle signed/unsigned comparison
			if constexpr (std::is_signed_v<T1> && std::is_unsigned_v<T2>) 
			{
				// Convert T1 (signed) to the corresponding unsigned type
				return b > static_cast<T2>(a < 0 ? 0 : a); // Handle negative values
			}
			else if constexpr (std::is_unsigned_v<T1> && std::is_signed_v<T2>) 
			{
				// Convert T2 (signed) to the corresponding unsigned type
				return static_cast<T1>(b < 0 ? 0 : b) > a; // Handle negative values
			}
			else 
			{
				return a > b; // Perform greater than or equal comparison
			}
		}
		else 
		{
			error("Greater than comparison can only be applied to integral types.");
			return false; // Return a default value or handle as needed
		}
	}, left, right);
}

token_value parser::bitwise_and(token_value& left, token_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> token_value 
	{
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are integral types (not floating-point)
		if constexpr (std::is_integral_v<T1> && !std::is_same_v<T1, bool> && std::is_integral_v<T2> && !std::is_same_v<T2, bool>) 
		{
			return a & b; // Perform bitwise AND operation
		}
		else 
		{
			error("Bitwise AND can only be applied to integral types.");
			return a; // Return a left value
		}
	}, left, right);
}

token_value parser::bitwise_exclusive_or(token_value& left, token_value& right)
{
	return std::visit([this](auto&& a, auto&& b) -> token_value 
		{
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are integral types (not floating-point)
		if constexpr (std::is_integral_v<T1> && !std::is_same_v<T1, bool> && std::is_integral_v<T2> && !std::is_same_v<T2, bool>) 
		{
			return a ^ b; // Perform bitwise XOR operation
		}
		else {
			error("Bitwise XOR can only be applied to integral types.");
			return a; // Return left value
		}
	}, left, right);
}

token_value parser::bitwise_or(token_value& left, token_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> token_value 
	{
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are integral types (not floating-point)
		if constexpr (std::is_integral_v<T1> && !std::is_same_v<T1, bool> && std::is_integral_v<T2> && !std::is_same_v<T2, bool>) 
		{
			return a | b; // Perform bitwise AND operation
		}
		else 
		{
			error("Bitwise AND can only be applied to integral types.");
			return a; // Return left value
		}
	}, left, right);
}

bool parser::logical_and(token_value &left, token_value &right) 
{
	return std::visit([this](auto&& a, auto&& b) -> bool 
	{
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are boolean types
		if constexpr (std::is_same_v<T1, bool> && std::is_same_v<T2, bool>) 
		{
			return a && b; // Perform logical AND operation
		}
		else if constexpr (std::is_same_v<T1, int> && std::is_same_v<T2, int>) 
		{
			return static_cast<bool>(a) && static_cast<bool>(b); // Perform logical OR operation
		}
		else if constexpr (std::is_same_v<T1, int> && std::is_same_v<T2, bool>) 
		{
			return static_cast<bool>(a) && b; // Perform logical OR operation
		}
		else if constexpr (std::is_same_v<T1, bool> && std::is_same_v<T2, int>) 
		{
			return a && static_cast<bool>(b); // Perform logical OR operation
		}
		else {
			error("Logical AND can only be applied to boolean types.");
			return false; // Return a default value or handle as needed
		}
	}, left, right);
}

bool parser::logical_or(token_value& left, token_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> bool 
	{
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// if both are boolean types
		if constexpr (std::is_same_v<T1, bool> && std::is_same_v<T2, bool>) 
		{
			return a || b; // Perform logical OR operation
		}
		else if constexpr (std::is_same_v<T1, int> && std::is_same_v<T2, int>) 
		{
			return static_cast<bool>(a) || static_cast<bool>(b); // Perform logical OR operation
		}
		else if constexpr (std::is_same_v<T1, int> && std::is_same_v<T2, bool>) 
		{
			return static_cast<bool>(a) || b; // Perform logical OR operation
		}
		else if constexpr (std::is_same_v<T1, bool> && std::is_same_v<T2, int>) 
		{
			return a || static_cast<bool>(b); // Perform logical OR operation
		}
		else {
			error("Logical OR can only be applied to boolean types.");
			return false; // Return a default value or handle as needed
		}
	}, left, right);
}

token_value parser::negate(token_value& value) 
{
	return std::visit([this](auto&& a) -> token_value 
	{
		using T = std::decay_t<decltype(a)>; // Get the type of the value

		// Check if the type is signed
		if constexpr (std::is_signed_v<T>) 
		{
			return -a; // Perform negation
		}
		else 
		{
			error("Negation can only be applied to signed types.");
			return token_value{}; // Return a default-constructed token_value or handle as needed
		}
	}, value);
}


token_value parser::not_(token_value& value) 
{
	return std::visit([](auto&& a) -> token_value 
	{
		return !a;
	}, value);
}

token_value parser::bitwise_not(token_value& value) 
{
	return std::visit([this](auto&& a) -> token_value 
	{
		using T = std::decay_t<decltype(a)>; // Get the type of a
		if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) 
		{ // Check if T is an integral type
			return ~a; // Apply bitwise NOT
		}
		else 
		{
			error("Bitwise NOT operation is only defined for integral types.");
			return 0; // Return a default value to satisfy the return type
		}
	}, value);
}

bool parser::check(token_kind expected_token_kind, const std::string& error_message)
{
	if (_token.kind != expected_token_kind)
	{
		error(error_message);
		return false;
	}
	scan();
	return true;
}

void parser::error(const std::string& message)
{
	if (_error_distance >= 3)
	{
		std::cerr << "Line " << _line << ", " << "pos " << _pos << ": " << message << std::endl;
		_errors++;
	}
	_error_distance = 0;
}

void parser::error(std::string&& message)
{
	if (_error_distance >= 3) 
	{
		std::cerr << "Line " << _line << ", " << "pos " << _pos << ": " << message << std::endl;
		_errors++;
	}
	_error_distance = 0;
}

parser::parser(const std::string& filename) : _scanner{ *this }
{
	std::ifstream expr_file{ filename };
	if (!expr_file)
	{
		std::cerr << "File not found" << std::endl;
		return;
	}
	std::stringstream source_stream;
	source_stream << expr_file.rdbuf();
	_source = source_stream.str();
	bool parse(token_value& value);
	_scanner.set_source(_source);
	_token = _scanner.next();
	_pos = _scanner.column();
	_line = _scanner.line();
	_lookahead_token = _scanner.next();
}

bool parser::parse(token_value& value)
{
	auto result{ true };
	if (!parse_expression(value))
		result = false;

	return result;
}

bool parser::parse_expression(token_value& value)
{
	auto result{ true };
	if (!parse_binary_expression(value))
		result = false;
	if (_token.kind != token_kind::END_OF_FILE)
	{
		error("Expression end expected");
		do
		{
			scan();
		} while (_token.kind == token_kind::END_OF_FILE);
		result = false;
	}
	return result;
}

bool parser::parse_binary_expression(token_value& value)
{
	auto result{ true };
	if (!parse_unary_expression(value))
		result = false;
	if (!parse_binary_expression_prime(value, operator_precedence::LOGICAL_OR))
		result = false;
	return result;
}

bool parser::parse_binary_expression_prime(token_value& left_value, operator_precedence minimal_precedence)
{
	auto result{ true };
	token_value right_value;
	operator_precedence op_prec;
	while (precedence::get_instance().is_binary_operator(_token, op_prec) && op_prec >= minimal_precedence) {
		token_kind op_token = _token.kind;
		scan();
		if (!parse_unary_expression(right_value))
			result = false;
		operator_precedence right_precedence;
		while (precedence::get_instance().is_binary_operator(_token, right_precedence) && right_precedence > op_prec)
		{
			if (!parse_binary_expression_prime(right_value, right_precedence))
				result = false;
		}
		switch (op_token)
		{
		case token_kind::STAR:
			left_value = multiply(left_value, right_value);
			break;
		case token_kind::SLASH:
			left_value = divide(left_value, right_value);
			break;
		case token_kind::PERCENT:
			left_value = modulus(left_value, right_value);
			break;
		case token_kind::PLUS:
			left_value = add(left_value, right_value);
			break;
		case token_kind::DASH:
			left_value = subtract(left_value, right_value);
			break;
		case token_kind::LESS_LESS:
			left_value = left_shift(left_value, right_value);
			break;
		case token_kind::GREATER_GREATER:
			left_value = right_shift(left_value, right_value);
			break;
		case token_kind::LESS_EQUAL:
			left_value = less_equal(left_value, right_value);
			break;
		case token_kind::GREATER_EQUAL:
			left_value = greater_equal(left_value, right_value);
			break;
		case token_kind::LESS:
			left_value = less(left_value, right_value);
			break;
		case token_kind::GREATER:
			left_value = greater(left_value, right_value);
			break;
		case token_kind::EQUAL_EQUAL:
			left_value = left_value == right_value;
			break;
		case token_kind::EXCLAIM_EQUAL:
			left_value = left_value != right_value;
			break;
		case token_kind::AMP:
			left_value = bitwise_and(left_value, right_value);
			break;
		case token_kind::CARET:
			left_value = bitwise_exclusive_or(left_value, right_value);
			break;
		case token_kind::BAR:
			left_value = bitwise_or(left_value, right_value);
			break;
		case token_kind::AMP_AMP:
			left_value = logical_and(left_value, right_value);
			break;
		case token_kind::BAR_BAR:
			left_value = logical_or(left_value, right_value);
			break;
		default:
			error("Unknown binary operator");
			result = false;
			break;
		}
	}

	return result;
}

bool parser::parse_hex_literal(token_value& value, bool& is_hex)
{
	const std::string& str = _scanner.value();
	size_t idx{ 0 };
	is_hex = false;

	// Check for hexadecimal format
	if (str.size() > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		is_hex = true;
		idx = 2; // Skip "0x" or "0X"
	}

	int result = true;
	if (!is_hex) {
		return result;
	}
	auto too_large{ false };
	// Parse the hexadecimal digits
	for (; idx < str.size(); ++idx)
	{
		if (idx == 18 - 1)
		{
			too_large = true;
			error("Hexadecimal literal too large");
			result = false;
		}
		char ch = str[idx];
		if (std::isdigit(ch)) {
			result = result * 16 + (ch - '0');
		}
		else if (ch >= 'a' && ch <= 'f') {
			result = result * 16 + (ch - 'a' + 10);
		}
		else if (ch >= 'A' && ch <= 'F') {
			result = result * 16 + (ch - 'A' + 10);
		}
		else {
			if (!too_large)
			{
				error("Invalid hexadecimal literal");
				result = false;
			}
		}
	}
	return result;
}

enum class integer_type
{
	CHAR,
	UNSIGNED_CHAR,
	SHORT,
	UNSIGNED_SHORT,
	INT,
	UNSIGNED_INT,
	LONG,
	UNSIGNED_LONG,
	LONG_LONG,
	UNSIGNED_LONG_LONG
};

static bool determine_integer_type(integer_type& integer_type, unsigned long long val, char ch)
{
	auto increment = ch - '0';
	switch (integer_type)
	{
	case integer_type::CHAR:
		if (val > (std::numeric_limits<char>::max() - increment) / 10)
		{
			integer_type = integer_type::UNSIGNED_CHAR;
			return determine_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::UNSIGNED_CHAR:
		if (val > (std::numeric_limits<unsigned char>::max() - increment) / 10)
		{
			integer_type = integer_type::SHORT;
			return determine_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::SHORT:
		if (val > (std::numeric_limits<short>::max() - increment) / 10)
		{
			integer_type = integer_type::UNSIGNED_SHORT;
			return determine_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::UNSIGNED_SHORT:

		if (val > (std::numeric_limits<unsigned short>::max() - increment) / 10)
		{
			integer_type = integer_type::INT;
			return determine_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::INT:
		if (val > (std::numeric_limits<int>::max() - increment) / 10)
		{
			integer_type = integer_type::UNSIGNED_INT;
			return determine_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::UNSIGNED_INT:
		if (val > (std::numeric_limits<unsigned int>::max() - increment) / 10)
		{
			integer_type = integer_type::LONG;
			return determine_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::LONG:
		if (val > (std::numeric_limits<long>::max() - increment) / 10)
		{
			integer_type = integer_type::UNSIGNED_LONG;
			return determine_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::UNSIGNED_LONG:
		if (val > (std::numeric_limits<unsigned long>::max() - increment) / 10)
		{
			integer_type = integer_type::LONG_LONG;
			return determine_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::LONG_LONG:
		if (val > (static_cast<unsigned long long>(std::numeric_limits<long long>::max()) - increment) / 10)
		{
			integer_type = integer_type::UNSIGNED_LONG_LONG;
			return determine_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::UNSIGNED_LONG_LONG:
		if (val > (std::numeric_limits<unsigned long long>::max() - increment) / 10)
			return false;
	}
	return true;
}

static bool determine_negative_integer_type(integer_type& integer_type, long long val, char ch)
{
	auto increment = ch - '0';
	switch (integer_type)
	{
	case integer_type::CHAR:
		if (val > (std::numeric_limits<char>::max() - increment) / 10)
		{
			integer_type = integer_type::SHORT;
			return determine_negative_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::SHORT:
		if (val > (std::numeric_limits<short>::max() - increment) / 10)
		{
			integer_type = integer_type::SHORT;
			return determine_negative_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::INT:
		if (val > (std::numeric_limits<int>::max() - increment) / 10)
		{
			integer_type = integer_type::LONG;
			return determine_negative_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::LONG:

		if (val > (-std::numeric_limits<long>::max() - increment) / 10)
		{
			integer_type = integer_type::LONG;
			return determine_negative_integer_type(integer_type, val, ch);
		}
		break;
	case integer_type::LONG_LONG:
		if (val > (std::numeric_limits<long long>::max() - increment) / 10)
			return false;
	}
	return true;
}

bool parser::parse_primary_expression(token_value& value)
{
	bool result{ true };
	switch (_token.kind)
	{
	case token_kind::INT_LITERAL:
	case token_kind::UNSIGNED_INT_LITERAL:
	case token_kind::LONG_LITERAL:
	case token_kind::UNSIGNED_LONG_LITERAL:
	case token_kind::LONG_LONG_LITERAL:
	case token_kind::UNSIGNED_LONG_LONG_LITERAL:
	case token_kind::FLOAT_LITERAL:
	case token_kind::DOUBLE_LITERAL:
		value = _token.value;
		scan();
		break;
	default:
		result = false; // No valid token found
	}
	return result;
}

bool parser::parse_unary_expression(token_value& value)
{
	bool result{ true };
	switch (_token.kind)
	{
	case token_kind::DASH:
		scan();
		result = parse_unary_expression(value);
		if (result)
			value = negate(value);
		break;
	case token_kind::PLUS:
		scan();
		result = parse_unary_expression(value);
		break;
	case token_kind::TILDE:
		scan();
		result = parse_unary_expression(value);
		if (result)
			value = bitwise_not(value);
		break;
	case token_kind::EXCLAIM:
		scan();
		result = parse_unary_expression(value);
		if (result)
			value = not_(value);
		break;
	default:
		if (_token.kind == token_kind::LPAREN)
		{
			scan();
			if (!parse_binary_expression(value))
				result = false;
			if (!check(token_kind::RPAREN, ") expected"))
				result = false;
			return result;
		}
		if (!is_primary_expression(_token))
		{
			error("Primary expression expected");
			result = false;
		}
		else
		{
			result = parse_primary_expression(value);
		}
		break;
	}

	return result;
}
