// expression.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <istream>
#include <iostream>
#include <sstream>

#include "parser.h"

int main()
{
	parser parser{ "first.exp" };
	expr_value value{ 0 };
	if (!parser.parse(value))
	{
		std::cerr << "Parsing failed." << std::endl;
		return EXIT_FAILURE;
	}
	std::visit([](auto&& arg) {
		std::cout << "Result: " << arg << std::endl;
		}, value);
	return EXIT_SUCCESS;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
