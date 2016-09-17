#include "Threading.hpp"

namespace Ausar
{
	namespace Threading
	{
		uint64_t ThreadTable::GetThreadIDByName(const char * Name) const
		{
			for (size_t i = 0; i < 64; i++)
			{
				if (Entries[i].Active)
				{
					if (std::strcmp(Entries[i].ThreadName, Name) == 0)
					{
						return Entries[i].ThreadID;
					}
				}
			}
			return -1;
		}
	}
}