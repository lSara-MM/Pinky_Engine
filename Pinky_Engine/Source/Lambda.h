#pragma once
#include "Globals.h"
#include "Light.h"

#include "Color.h"
#include "Application.h"

#include <functional>

// Define a struct with a variadic template for the function member
template<typename ReturnType, typename... Args>
struct Function
{
	std::string name;
	std::function<ReturnType(Args...)> myFunction;

	// Constructor to initialize the function member
	Function(std::function<ReturnType(Args...)> func) : myFunction(func) {}

	void SetFunction(std::function<ReturnType(Args...)> func)
	{
		myFunction = func;
	};

	// Method to use the stored function
	ReturnType ExecuteFunction(Args... args) 
	{
		return myFunction(args...);
	}
};

void LogSomething()
{
	LOG("Something");
}