#include "DebugMsg.h"

#include <iostream>

DebugMessenger::DebugMessenger() {}

void DebugMessenger::message(msgType header, std::string message) {
	std::string headerStr = "";

	switch (header) {
		case Note:
			headerStr = "NOTE";
			break;
		case Warning:
			headerStr = "WARNING";
			break;
		case Error:
			headerStr = "ERROR";
			break;
		case Debug:
		default:
			headerStr = "DEBUG";
			break;
	}

	std::cout << "[" << headerStr << "] -- " << message << std::endl;
}