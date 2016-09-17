#pragma once
#include <windows.h>
#include <iphlpapi.h>
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>

namespace Util
{
	namespace Debug
	{
		std::string GetStackTraceString(int framesToSkip, int framesToCapture);
	}
}