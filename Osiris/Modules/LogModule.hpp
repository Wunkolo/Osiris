#pragma once
#include "OsirisModule.hpp"

class LogModule : public OsirisModule
{
public:
	LogModule();
	~LogModule();

	static void LogMessage(const char* sourceFile, uint32_t sourceLine, uint32_t severity, uint32_t type, const char* message, ...);

	void Tick(const std::chrono::high_resolution_clock::duration &DeltaTime);
private:
};