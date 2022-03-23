#include "app.h"
#include <stdexcept>
#include <iostream>
#include <fstream>


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
		std::ofstream crashFile("CrashInfo.txt");
		if (crashFile.is_open())
		{
			crashFile.seekp(0);
			crashFile << e.what();
			crashFile.close();
		}
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
