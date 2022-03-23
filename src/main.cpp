#include "app.h"
#include <stdexcept>
#include <iostream>


int main()
{
	try
	{
		#ifndef NDEBUG
		std::cout << "In debug mode" << std::endl;
		#endif
		App app;
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		int wait;
		std::cin >> wait;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
