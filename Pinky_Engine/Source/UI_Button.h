#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"
#include "UI_Image.h"

//#include "Lambda.h"
#include <iostream>
#include <functional>
#include <vector>

class UI_Button : public C_UI
{
public:

	UI_Button(GameObject* g, int w, int h, int x, int y);
	~UI_Button();

	void ShowInInspector();

	void OnNormal() override;
	void OnFocused() override;
	void OnPressed() override;
	void OnSelected() override;
	void OnRelease() override;

	// Define a function wrapper type using std::function
	using FunctionType = std::function<void()>;

	// Member functions to set the functions for different states
	void SetNormalFunction(FunctionType func);
	void SetFocusedFunction(FunctionType func);
	void SetPressedFunction(FunctionType func);
	void SetSelectedFunction(FunctionType func);

	//// Define a function wrapper type using std::function
	//template <typename FunctionType>
	//using FunctionWrapper = std::function<FunctionType>;

	//// Constructor that takes a function wrapper as a parameter
	//template <typename FunctionType>
	//UI_Button(FunctionWrapper<FunctionType> func) : myFunction(func) {}

	//// Member function to set the function wrapper
	//template <typename FunctionType>
	//void setFunction(FunctionWrapper<FunctionType> func) {
	//	myFunction = func;
	//}

	//// Member function to call the stored function
	//void callFunction() {
	//	if (myFunction) {
	//		myFunction();
	//	}
	//	else {
	//		std::cout << "No function set." << std::endl;
	//	}
	//}

	//// Define a function pointer type
	//using Function = void(*)();

	//// Member function to set the function pointer
	//void SetFunction(Function func) 
	//{
	//	function = func;
	//}

	//// Member function to call the stored function
	//void CallFunction() 
	//{
	//	if (function) {
	//		function();
	//	}
	//	else {
	//		std::cout << "No function set." << std::endl;
	//	}
	//}

public:
	bool isInteractable;
	UI_Image* image;
	std::function<void()> function;

	// Function wrappers for different states
	FunctionType normalFunction;
	FunctionType focusedFunction;
	FunctionType pressedFunction;
	FunctionType selectedFunction;

	// Helper function to execute a function if it's set
	void ExecuteFunction(const FunctionType& func);
private:
	//color
	Color focusedColor;
	Color pressedColor;
	Color selectedColor;
	Color disabledColor;

	// Function wrapper variable
	//std::function<void()> myFunction;
	// Function pointer variable
	//FunctionType function;
	//Function<void> function = nullptr;
};
#endif // __UI_BUTTON_H__