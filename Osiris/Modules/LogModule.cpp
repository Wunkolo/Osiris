#include "LogModule.hpp"

#include <Ausar/Ausar.hpp>
#include <Utils/Utils.hpp>
#include <stdarg.h>

LogModule::LogModule()
{
	// empty zip password
	Util::Process::GetModuleBase()(0x3344630).Write<uint8_t>(0);

	// EnableReleaseLogShim and hook the main ingame logger
	bool redirectGameLogs = false;	// TODO: break this out into an external config option
	Util::Process::GetModuleBase()(0x58938C8).Write<uint8_t>(redirectGameLogs);
	Util::Process::GetModuleBase()(0x58938D0).Write<void*>(LogMessage);

	LOG << "Log Module Loaded" << std::endl;
}

LogModule::~LogModule()
{

}

void LogModule::Tick(const std::chrono::high_resolution_clock::duration & DeltaTime)
{
	static std::chrono::high_resolution_clock::duration Timer
		= std::chrono::high_resolution_clock::duration::zero();

	Timer += DeltaTime;
}

// TODO: I'm currently guessing on the severity and type parameters, they'll need proper flags/enums created eventually
void LogModule::LogMessage(const char* sourceFile, uint32_t sourceLine, uint32_t severity, uint32_t type, const char* message, ...)
{
	// construct formatted message
	va_list ap;
	va_start(ap, message);
	int bufferSize = _vscprintf(message, ap) + 1;
	char* buffer = new char[bufferSize];
	vsprintf_s(buffer, bufferSize, message, ap);
	va_end(ap);

	LOG << sourceFile << ":" << std::dec << sourceLine << " - " << buffer << std::endl;
}