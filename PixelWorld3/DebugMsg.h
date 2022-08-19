#pragma once
#include <string>
class DebugMessenger {
	public:
		enum msgType {Note,Warning,Error,Debug};

		DebugMessenger();
		static void message(msgType header, std::string message);
};