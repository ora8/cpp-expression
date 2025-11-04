#include <fstream>
#include <iostream>
#include <sstream>

#include "parser.h"
#include "precedence.h"
#include "scanner.h"

expr_value add(expr_value lhs, expr_value rhs) {
	return std::visit([](auto&& a, auto&& b) -> expr_value {
		return a + b;
		}, lhs, rhs);
}

expr_value subtract(expr_value lhs, expr_value rhs) {
	return std::visit([](auto&& a, auto&& b) -> expr_value {
		return a - b;
		}, lhs, rhs);
}

expr_value multiply(expr_value lhs, expr_value rhs) {
	return std::visit([](auto&& a, auto&& b) -> expr_value {
		return a * b;
		}, lhs, rhs);
}

expr_value divide(scanner scanner, expr_value lhs, expr_value rhs) {
	return std::visit([&scanner](auto&& a, auto&& b) -> expr_value {
		using T1 = std::decay_t<decltype(a)>;
		using T2 = std::decay_t<decltype(b)>;
		// Check if both types are compatible for modulus
		if constexpr (std::is_arithmetic_v<T1> && !std::is_same_v<T1, bool> &&
			std::is_arithmetic_v<T2> && !std::is_same_v<T2, bool>) {
			return a / b; // Perform modulus operation
		}
		else {
			scanner.error("Divide operation is only defined for int, long, unsigned long long, flaot and double.");
			return 0; // Return a default value to satisfy the return type
		}
		}, lhs, rhs);
}

expr_value modulus(scanner scanner, expr_value lhs, expr_value rhs) {
	return std::visit([&scanner](auto&& a, auto&& b) -> expr_value {
		using T1 = std::decay_t<decltype(a)>;
		using T2 = std::decay_t<decltype(b)>;

		// Check if both types are compatible for modulus
		if constexpr ((std::is_same_v<T1, int> || std::is_same_v<T1, long> || std::is_same_v<T1, unsigned long long>) &&
			(std::is_same_v<T2, int> || std::is_same_v<T2, long> || std::is_same_v<T2, unsigned long long>)) {
			return a % b; // Perform modulus operation
		}
		else {
			scanner.error("Modulus operation is only defined for int, long, and unsigned long long.");
			return 0; // Return a default value to satisfy the return type
		}
		}, lhs, rhs);
}

expr_value left_shift(scanner& scanner, expr_value left, expr_value right) {
	return std::visit([&scanner](auto&& a, auto&& b) -> expr_value {
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both lhs and rhs are integral types and rhs is non-negative and not boolean
		if constexpr (std::is_integral_v<T1> && !std::is_same_v<T1, bool> &&
			std::is_integral_v<T2> && !std::is_same_v<T2, bool>) {
			if (b < 0) {
				scanner.error("Right-hand side must be non-negative for left shift.");
				return a; // Return the original value or handle as needed
			}
			return a << b; // Perform left bitwise shift
		}
		else {
			scanner.error("Left shift can only be applied to integral types.");
			return a; // Return the original value or handle as needed
		}
		}, left, right);
}

expr_value right_shift(scanner scanner, expr_value left, expr_value right) {
	return std::visit([&scanner](auto&& a, auto&& b) -> expr_value {
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure rhs is an integral type and non-negative
		if constexpr (std::is_integral_v<T1> && !std::is_same_v<T1, bool> &&
			std::is_integral_v<T2> && !std::is_same_v<T2, bool>) {
			if (b < 0) {
				scanner.error("Right-hand side must be non-negative for right shift.");
				return a;
			}
			return a >> b; // Perform right bitwise shift
		}
		else {
			scanner.error("Left shift can only be applied to integral types.");
			return a;
		}
		}, left, right);
}

bool less_equal(scanner& scanner, expr_value left, expr_value right) {
	return std::visit([&scanner](auto&& a, auto&& b) -> bool {
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are integral types
		if constexpr (std::is_arithmetic_v<T1> && !std::is_same_v<T1, bool> && std::is_arithmetic_v<T2> && !std::is_same_v<T2, bool>) {
			return a <= b; // Perform less than or equal comparison
		}
		else {
			scanner.error("Less than or equal comparison can only be applied to integral types.");
			return false; // Return a default value or handle as needed
		}
		}, left, right);
}

bool greater_equal(scanner& scanner, expr_value left, expr_value right) {
	return std::visit([&scanner](auto&& a, auto&& b) -> bool {
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are integral types
		if constexpr (std::is_arithmetic_v<T1> && !std::is_same_v<T1, bool> && std::is_arithmetic_v<T2> && !std::is_same_v<T2, bool>) {
			return a >= b; // Perform greater than or equal comparison
		}
		else {
			scanner.error("Greater than or equal comparison can only be applied to integral types.");
			return false; // Return a default value or handle as needed
		}
		}, left, right);
}

bool less(scanner& scanner, expr_value left, expr_value right) {
	return std::visit([&scanner](auto&& a, auto&& b) -> bool {
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are integral types
		if constexpr (std::is_arithmetic_v<T1> && !std::is_same_v<T1, bool> && std::is_arithmetic_v<T2> && !std::is_same_v<T2, bool>) {
			return a < b; // Perform less than comparison
		}
		else {
			scanner.error("Less than comparison can only be applied to integral types.");
			return false; // Return a default value or handle as needed
		}
		}, left, right);
}

bool greater(scanner& scanner, expr_value left, expr_value right) {
	return std::visit([&scanner](auto&& a, auto&& b) -> bool {
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are integral types
		if constexpr (std::is_arithmetic_v<T1> && !std::is_same_v<T1, bool> && std::is_arithmetic_v<T2> && !std::is_same_v<T2, bool>) {
			return a > b; // Perform greater than comparison
		}
		else {
			scanner.error("Greater than comparison can only be applied to integral types.");
			return false; // Return a default value or handle as needed
		}
		}, left, right);
}

expr_value bitwise_and(scanner& scanner, expr_value left, expr_value right) {
	return std::visit([&scanner](auto&& a, auto&& b) -> expr_value {
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are integral types (not floating-point)
		if constexpr (std::is_integral_v<T1> && !std::is_same_v<T1, bool> && std::is_integral_v<T2> && !std::is_same_v<T2, bool>) {
			return a & b; // Perform bitwise AND operation
		}
		else {
			scanner.error("Bitwise AND can only be applied to integral types.");
			return a; // Return a left value
		}
		}, left, right);
}

expr_value bitwise_exclusive_or(scanner& scanner, expr_value left, expr_value right) {
	return std::visit([&scanner](auto&& a, auto&& b) -> expr_value {
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are integral types (not floating-point)
		if constexpr (std::is_integral_v<T1> && !std::is_same_v<T1, bool> && std::is_integral_v<T2> && !std::is_same_v<T2, bool>) {
			return a ^ b; // Perform bitwise XOR operation
		}
		else {
			scanner.error("Bitwise XOR can only be applied to integral types.");
			return a; // Return left value
		}
		}, left, right);
}

expr_value bitwise_or(scanner& scanner, expr_value left, expr_value right) {
	return std::visit([&scanner](auto&& a, auto&& b) -> expr_value {
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are integral types (not floating-point)
		if constexpr (std::is_integral_v<T1> && !std::is_same_v<T1, bool> && std::is_integral_v<T2> && !std::is_same_v<T2, bool>) {
			return a | b; // Perform bitwise AND operation
		}
		else {
			scanner.error("Bitwise AND can only be applied to integral types.");
			return a; // Return left value
		}
		}, left, right);
}

bool logical_and(scanner& scanner, expr_value left, expr_value right) {
	return std::visit([&scanner](auto&& a, auto&& b) -> bool {
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// Ensure both are boolean types
		if constexpr (std::is_same_v<T1, bool> && std::is_same_v<T2, bool>) {
			return a && b; // Perform logical AND operation
		}
		else if constexpr (std::is_same_v<T1, int> && std::is_same_v<T2, int>) {
			return static_cast<bool>(a) && static_cast<bool>(b); // Perform logical OR operation
		}
		else if constexpr (std::is_same_v<T1, int> && std::is_same_v<T2, bool>) {
			return static_cast<bool>(a) && b; // Perform logical OR operation
		}
		else if constexpr (std::is_same_v<T1, bool> && std::is_same_v<T2, int>) {
			return a && static_cast<bool>(b); // Perform logical OR operation
		}
		else {
			scanner.error("Logical AND can only be applied to boolean types.");
			return false; // Return a default value or handle as needed
		}
		}, left, right);
}

bool logical_or(scanner& scanner, expr_value left, expr_value right) {
	return std::visit([&scanner](auto&& a, auto&& b) -> bool {
		using T1 = std::decay_t<decltype(a)>; // Get the type of lhs
		using T2 = std::decay_t<decltype(b)>; // Get the type of rhs

		// if both are boolean types
		if constexpr (std::is_same_v<T1, bool> && std::is_same_v<T2, bool>) {
			return a || b; // Perform logical OR operation
		}
		else if constexpr (std::is_same_v<T1, int> && std::is_same_v<T2, int>) {
			return static_cast<bool>(a) || static_cast<bool>(b); // Perform logical OR operation
		}
		else if constexpr (std::is_same_v<T1, int> && std::is_same_v<T2, bool>) {
			return static_cast<bool>(a) || b; // Perform logical OR operation
		}
		else if constexpr (std::is_same_v<T1, bool> && std::is_same_v<T2, int>) {
			return a || static_cast<bool>(b); // Perform logical OR operation
		}
		else {
			scanner.error("Logical OR can only be applied to boolean types.");
			return false; // Return a default value or handle as needed
		}
		}, left, right);
}

expr_value negate(scanner& scanner, expr_value value) {
	return std::visit([&scanner](auto&& a) -> expr_value {
		using T = std::decay_t<decltype(a)>; // Get the type of the value

		// Check if the type is signed
		if constexpr (std::is_signed_v<T>) {
			return -a; // Perform negation
		}
		else {
			scanner.error("Negation can only be applied to signed types.");
			return expr_value{}; // Return a default-constructed expr_value or handle as needed
		}
		}, value);
}


expr_value not_(expr_value value) {
	return std::visit([](auto&& a) -> expr_value {
		return !a;
		}, value);
}

expr_value bitwise_not(scanner scanner, expr_value value) {
	return std::visit([&scanner](auto&& a) -> expr_value {
		using T = std::decay_t<decltype(a)>; // Get the type of a
		if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) { // Check if T is an integral type
			return ~a; // Apply bitwise NOT
		}
		else {
			scanner.error("Bitwise NOT operation is only defined for integral types.");
			return 0; // Return a default value to satisfy the return type
		}
		}, value);
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
			left_value = divide(_scanner, left_value, right_value);
			break;
		case token::PERCENT:
			left_value = modulus(_scanner, left_value, right_value);
			break;
		case token::PLUS:
			left_value = add(left_value, right_value);
			break;
		case token::MINUS:
			left_value = subtract(left_value, right_value);
			break;
		case token::LESS_LESS:
			left_value = left_shift(_scanner, left_value, right_value);
			break;
		case token::GREATER_GREATER:
			left_value = right_shift(_scanner, left_value, right_value);
			break;
		case token::LESS_EQUAL:
			left_value = less_equal(_scanner, left_value, right_value);
			break;
		case token::GREATER_EQUAL:
			left_value = greater_equal(_scanner, left_value, right_value);
			break;
		case token::LESS:
			left_value = less(_scanner, left_value, right_value);
			break;
		case token::GREATER:
			left_value = greater(_scanner, left_value, right_value);
			break;
		case token::EQUAL_EQUAL:
			left_value = left_value == right_value;
			break;
		case token::EXCLAIM_EQUAL:
			left_value = left_value != right_value;
			break;
		case token::AMP:
			left_value = bitwise_and(_scanner, left_value, right_value);
			break;
		case token::CARET:
			left_value = bitwise_exclusive_or(_scanner, left_value, right_value);
			break;
		case token::BAR:
			left_value = bitwise_or(_scanner, left_value, right_value);
			break;
		case token::AMP_AMP:
			left_value = logical_and(_scanner, left_value, right_value);
			break;
		case token::BAR_BAR:
			left_value = logical_or(_scanner, left_value, right_value);
			break;
		}
	}

	return result;
}

bool parser::parse_unary_expression(expr_value& value)
{
	bool result{ true };
	switch (lookahead_token())
	{
	case token::MINUS:
		consume_token();
		result = parse_binary_expression(value);
		if (result)
			value = negate(_scanner, value);
		break;
	case token::PLUS:
		consume_token();
		result = parse_binary_expression(value);
		break;
	case token::TILDE:
		consume_token();
		result = parse_binary_expression(value);
		if (result)
			value = bitwise_not(_scanner, value);
		break;
	case token::EXCLAIM:
		consume_token();
		result = parse_binary_expression(value);
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
				_scanner.error(") expected");
				result = false;
			}
			return result;
		}
		if (lookahead_token() != token::NUMBER)
		{
			_scanner.error("Primary expression expected");
			result = false;
		}
		else
		{
			value = _scanner.number();
			consume_token();
			result = true;
		}
		break;
	}

	return result;
}
