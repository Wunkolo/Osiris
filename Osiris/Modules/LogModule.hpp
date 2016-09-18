#pragma once
#include "OsirisModule.hpp"

class LogModule : public OsirisModule
{
public:
	LogModule();
	~LogModule();

	static void LogMessage(const char* sourceFile, uint64_t sourceLine, uint64_t severity, uint64_t type, const char* message, ...);

	void Tick(const std::chrono::high_resolution_clock::duration &DeltaTime);
private:
};