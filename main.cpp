#include <iostream>

#include "Application.h"

void *operator new(size_t size)
{
	std::cout << "Allocating: " << size << " bytes\n";
	return malloc(size);
}

int main()
{
	Application app;
	app.Run();
}