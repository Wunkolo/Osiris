#include "Threading.hpp"
#include <stdint.h>
#include <string>
#include "Utils/Thread.hpp"

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

		// Returns the slot 0 TLS address of the main thread
		Util::Pointer GetMainTls()
		{
			if (_mainThreadTlsBase == nullptr)
			{
				const Ausar::Threading::ThreadTable *Table;
				Table = Util::Process::GetModuleBase()(0x58CA4B0).Point<Ausar::Threading::ThreadTable>();
				uint32_t ThreadID = Table->GetThreadIDByName("MAIN");
				Util::Thread::GetThreadLocalStorage(ThreadID, 0, _mainThreadTlsBase);
			}
			return _mainThreadTlsBase;
		}
    }
}