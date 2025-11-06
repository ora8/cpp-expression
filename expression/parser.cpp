#include <fstream>
#include <iostream>
#include <sstream>

#include "parser.h"
#include "precedence.h"
#include "scanner.h"

expr_value parser::add(expr_value& lhs, expr_value& rhs) 
{
	return std::visit([](auto&& a, auto&& b) -> expr_value 
	{
		return a + b;
	}, lhs, rhs);
}

expr_value parser::subtract(expr_value& lhs, expr_value& rhs) 
{
	return std::visit([](auto&& a, auto&& b) -> expr_value 
	{
		return a - b;
	}, lhs, rhs);
}

expr_value parser::multiply(expr_value& lhs, expr_value& rhs) 
{
	return std::visit([](auto&& a, auto&& b) -> expr_value 
	{
		return a * b;
	}, lhs, rhs);
}

expr_value parser::divide(expr_value& lhs, expr_value& rhs) 
{
	return std::visit([this](auto&& a, auto&& b) -> expr_value 
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

expr_value parser::modulus(expr_value& lhs, expr_value& rhs) 
{
	return std::visit([this](auto&& a, auto&& b) -> expr_value 
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

expr_value parser::left_shift(expr_value& left, expr_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> expr_value 
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

expr_value parser::right_shift(expr_value& left, expr_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> expr_value 
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

bool parser::less_equal(expr_value& left, expr_value& right) 
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

bool parser::greater_equal(expr_value& left, expr_value& right) 
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

bool parser::less(expr_value& left, expr_value& right) 
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

bool parser::greater(expr_value& left, expr_value& right) 
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

expr_value parser::bitwise_and(expr_value& left, expr_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> expr_value 
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

expr_value parser::bitwise_exclusive_or(expr_value& left, expr_value& right)
{
	return std::visit([this](auto&& a, auto&& b) -> expr_value 
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

expr_value parser::bitwise_or(expr_value& left, expr_value& right) 
{
	return std::visit([this](auto&& a, auto&& b) -> expr_value 
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

bool parser::logical_and(expr_value &left, expr_value &right) 
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

bool parser::logical_or(expr_value& left, expr_value& right) 
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

expr_value parser::negate(expr_value& value) 
{
	return std::visit([this](auto&& a) -> expr_value 
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
			return expr_value{}; // Return a default-constructed expr_value or handle as needed
		}
	}, value);
}


expr_value parser::not_(expr_value& value) 
{
	return std::visit([](auto&& a) -> expr_value 
	{
		return !a;
	}, value);
}

expr_value parser::bitwise_not(expr_value& value) 
{
	return std::visit([this](auto&& a) -> expr_value 
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

void parser::error(const std::string& message)
{
	std::cerr << "Line " << _scanner.line() << ", " << "pos " << _scanner.location() << ": " << message << std::endl;
}

void parser::error(std::string&& message)
{
	std::cerr << "Line " << _scanner.line() << ", " << "pos " << _scanner.location() << ": " << message << std::endl;
}

parser::parser(std::string file_name)
{
	std::ifstream expr_file{ "first.exp" };
	if (!expr_file)
	{
		std::cerr << "File not found" << std::endl;
		return;
	}
	std::stringstream source_stream;
	source_stream << expr_file.rdbuf();
	_source = source_stream.str();
}

bool parser::parse(expr_value& value)
{
	_scanner.set_source(_source);
	consume_token();
	auto result{ true };
	if (!parse_expression(value))
		result = false;

	return result;
}

bool parser::parse_expression(expr_value& value)
{
	auto result{ true };
	if (!parse_binary_expression(value))
		result = false;
	if (lookahead_token() != token::END_OF_FILE)
	{
		while (lookahead_token() != token::END_OF_FILE)
			consume_token();
		result = false;
	}
	return result;
}

bool parser::parse_binary_expression(expr_value& value)
{
	auto result{ true };
	if (!parse_unary_expression(value))
		result = false;
	if (!parse_binary_expression_prime(value, operator_precedence::LOGICAL_OR))
		result = false;
	return result;
}

bool parser::parse_binary_expression_prime(expr_value& left_value, operator_precedence minimal_precedence)
{
	auto result{ true };
	expr_value right_value;
	operator_precedence op_prec;
	while (precedence::get_instance().is_binary_operator(lookahead_token(), op_prec) && op_prec >= minimal_precedence) {
		token op_token = lookahead_token();
		consume_token();
		if (!parse_unary_expression(right_value))
			result = false;
		operator_precedence right_precedence;
		while (precedence::get_instance().is_binary_operator(lookahead_token(), right_precedence) && right_precedence > op_prec)
		{
			if (!parse_binary_expression_prime(right_value, right_precedence))
				result = false;
		}
		switch (op_token)
		{
		case token::STAR:
			left_value = multiply(left_value, right_value);
			break;
		case token::SLASH:
			left_value = divide(left_value, right_value);
			break;
		case token::PERCENT:
			left_value = modulus(left_value, right_value);
			break;
		case token::PLUS:
			left_value = add(left_value, right_value);
			break;
		case token::MINUS:
			left_value = subtract(left_value, right_value);
			break;
		case token::LESS_LESS:
			left_value = left_shift(left_value, right_value);
			break;
		case token::GREATER_GREATER:
			left_value = right_shift(left_value, right_value);
			break;
		case token::LESS_EQUAL:
			left_value = less_equal(left_value, right_value);
			break;
		case token::GREATER_EQUAL:
			left_value = greater_equal(left_value, right_value);
			break;
		case token::LESS:
			left_value = less(left_value, right_value);
			break;
		case token::GREATER:
			left_value = greater(left_value, right_value);
			break;
		case token::EQUAL_EQUAL:
			left_value = left_value == right_value;
			break;
		case token::EXCLAIM_EQUAL:
			left_value = left_value != right_value;
			break;
		case token::AMP:
			left_value = bitwise_and(left_value, right_value);
			break;
		case token::CARET:
			left_value = bitwise_exclusive_or(left_value, right_value);
			break;
		case token::BAR:
			left_value = bitwise_or(left_value, right_value);
			break;
		case token::AMP_AMP:
			left_value = logical_and(left_value, right_value);
			break;
		case token::BAR_BAR:
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

bool parser::parse_hex_literal(expr_value& value, bool& is_hex)
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

static void set_integer_value(expr_value& value, integer_type integer_type, unsigned long long val)
{
	switch (integer_type)
	{
	case integer_type::CHAR:
	case integer_type::UNSIGNED_CHAR:
	case integer_type::SHORT:
	case integer_type::UNSIGNED_SHORT:
	case integer_type::INT:
		value = static_cast<int>(val);
		break;
	case integer_type::UNSIGNED_INT:
		value = static_cast<unsigned int>(val);
		break;
	case integer_type::LONG:
		value = static_cast<long>(val);
		break;
	case integer_type::UNSIGNED_LONG:
		value = static_cast<unsigned long>(val);
		break;
	case integer_type::LONG_LONG:
		value = static_cast<long long>(val);
		break;
	case integer_type::UNSIGNED_LONG_LONG:
		value = static_cast<unsigned long long>(val);
		break;
	}
}

static void set_negative_integer_value(expr_value& value, integer_type integer_type, long long val)
{
	switch (integer_type)
	{
	case integer_type::CHAR:
	case integer_type::SHORT:
	case integer_type::INT:
		value = static_cast<int>(val);
		break;
	case integer_type::LONG:
		value = static_cast<long>(val);
		break;
	case integer_type::LONG_LONG:
		value = static_cast<long long>(val);
		break;
	}
}

bool parser::parse_integer_literal(expr_value& value)
{
	auto is_hex{ false };
	parse_hex_literal(value, is_hex);
	if (is_hex)
		return true;

	std::string str = _scanner.value();
	unsigned long long val{ 0 };
	integer_type integer_type{ integer_type::CHAR };
	long long neg_val{ 0 };
	// Check if negative
	auto is_neg = str.size() > 0 && str[0] == '-' ? true : false;
	auto start_idx = is_neg ? 1 : 0;
	auto too_large{ false };
	for (size_t idx = start_idx; idx < str.size(); ++idx)
	{
		char ch = str[idx];
		if (std::isdigit(ch))
		{
			if (is_neg)
			{
				if (determine_negative_integer_type(integer_type, neg_val, ch))
				{
					if (neg_val == 0)
						neg_val = -(ch - '0');
					else
						neg_val = neg_val * 10 + (ch - '0');
				}
				else
				{
					if (!too_large)
						error("Integer literal too large");
				}
			}
			else
			{
				if (determine_integer_type(integer_type, val, ch))
					val = val * 10 + (ch - '0');
				else
				{
					if (!too_large)
						error("Integer literal too large");
				}
			}
		}
		else
		{
			error("Invalid decimal literal");
			return false;
		}
	}
	if (is_neg)
		set_negative_integer_value(value, integer_type, neg_val);
	else
		set_integer_value(value, integer_type, val);
	return true;
}

static bool stod_negative(const std::string& str, size_t idx, double& val)
{
	auto result{ true };
	val = 0.0;

	// Parse integer part
	while (idx < str.size() && std::isdigit(str[idx]))
	{
		auto increment = str[idx] - '0';
		if (val < (std::numeric_limits<double>::min() + increment) / 10.0)
		{
			result = false;
			break;
		}
		if (idx == 0)
			val = -increment;
		else
			val = val * 10.0 - increment;
		++idx;
	}

	// Parse fractional part
	if (idx < str.size() && str[idx] == '.') {
		++idx;
		double fraction = 1.0;
		while (idx < str.size() && std::isdigit(str[idx])) {
			fraction *= 0.1;
			val += (str[idx] - '0') * fraction;
			++idx;
		}
	}

	// Handle exponent part
	if (idx < str.size() && (str[idx] == 'e' || str[idx] == 'E')) {
		++idx;
		bool exp_negative{ false };
		if (idx < str.size() && str[idx] == '-') {
			exp_negative = true;
			++idx;
		}
		else if (idx < str.size() && str[idx] == '+') {
			++idx;
		}

		int exponent = 0;
		while (idx < str.size() && std::isdigit(str[idx])) {
			exponent = exponent * 10 + (str[idx] - '0');
			++idx;
		}
		if (exp_negative) {
			exponent = -exponent;
		}
		for (auto i = 0; i < exponent; ++i)
		{
			if (val < std::numeric_limits<double>::min() / 10)
			{
				result = false;
				break;
			}
			val *= 10;
		}
	}
	return result;
}

static bool stod(const std::string& str, double& val) 
{
	auto result{ true };
	size_t idx = 0;
	val = 0.0;

	// Check for sign
	if (idx < str.size() && str[idx] == '-') 
	{
		++idx;
		return stod_negative(str, idx, val);
	}
	if (idx < str.size() && str[idx] == '+')
		++idx;

	// Parse integer part
	while (idx < str.size() && std::isdigit(str[idx])) 
	{
		auto increment = str[idx] - '0';
		if (val > (std::numeric_limits<double>::max() - increment) / 10.0)
		{
			result = false;
			break;
		}	
		val = val * 10.0 + increment;
		++idx;
	}

	// Parse fractional part
	if (idx < str.size() && str[idx] == '.') {
		++idx;
		double fraction = 1.0;
		while (idx < str.size() && std::isdigit(str[idx])) {
			fraction *= 0.1;
			val += (str[idx] - '0') * fraction;
			++idx;
		}
	}

	// Handle exponent part
	if (idx < str.size() && (str[idx] == 'e' || str[idx] == 'E')) {
		++idx;
		bool exp_negative = false;
		if (idx < str.size() && str[idx] == '-') {
			exp_negative = true;
			++idx;
		}
		else if (idx < str.size() && str[idx] == '+') {
			++idx;
		}

		int exponent = 0;
		while (idx < str.size() && std::isdigit(str[idx])) {
			exponent = exponent * 10 + (str[idx] - '0');
			++idx;
		}
		if (exp_negative) {
			exponent = -exponent;
		}
		for (auto i = 0; i < exponent; ++i)
		{
			if (val > std::numeric_limits<double>::max() / 10)
			{
				result = false;
				break;
			}
			val *= 10;
		}
	}

	return result;
}

bool parser::parse_primary_expression(expr_value& value)
{
	bool result{ true };
	switch (lookahead_token())
	{
	case token::INTEGER_LITERAL:
		result = parse_integer_literal(value);
		if (result)
			consume_token();
		return result;
	case token::FLOAT_LITERAL:
		double float_value;
		try
		{
			float_value = std::stod(_scanner.value());
			if (float_value <= std::numeric_limits<float>::max())
				value = static_cast<float>(float_value);
			else
				value = float_value; // This will handle larger values
			consume_token();
			return true; // Successfully parsed float literal
		}
		catch (const std::exception&)
		{
			error("Invalid float literal");
			return false;
		}
	default:
		return false; // No valid token found
	}
}

bool parser::parse_unary_expression(expr_value& value)
{
	bool result{ true };
	switch (lookahead_token())
	{
	case token::MINUS:
		consume_token();
		result = parse_unary_expression(value);
		if (result)
			value = negate(value);
		break;
	case token::PLUS:
		consume_token();
		result = parse_unary_expression(value);
		break;
	case token::TILDE:
		consume_token();
		result = parse_unary_expression(value);
		if (result)
			value = bitwise_not(value);
		break;
	case token::EXCLAIM:
		consume_token();
		result = parse_unary_expression(value);
		if (result)
			value = not_(value);
		break;
	default:
		if (lookahead_token() == token::LPAREN)
		{
			consume_token();
			if (!parse_binary_expression(value))
				result = false;
			if (lookahead_token() == token::RPAREN)
				consume_token();
			else
			{
				error(") expected");
				result = false;
			}
			return result;
		}
		if (!is_primary_expression(lookahead_token()))
		{
			error("Primary expression expected");
			result = false;
		}
		else
		{
			result = parse_primary_expression(value);
			result = true;
		}
		break;
	}

	return result;
}
