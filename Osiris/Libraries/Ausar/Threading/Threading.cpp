#include "Threading.hpp"
#include <string>

namespace Ausar
{
    namespace Threading
    {
        uint32_t ThreadTable::GetThreadIDByName(const char * Name) const
        {
            for( size_t i = 0; i < 64; i++ )
            {
                if( Entries[i].Active )
                {
                    if( std::strcmp(Entries[i].ThreadName, Name) == 0 )
                    {
                        return static_cast<uint32_t>(Entries[i].ThreadID);
                    }
                }
            }
            return -1;
        }
    }
}