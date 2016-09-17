#include <windows.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <stdlib.h>
#include <locale>
#include <codecvt>
#include <conio.h>
#include <psapi.h> //GetModuleFileNameEx

// Setting DLL access controls
#include <AccCtrl.h>
#include <Aclapi.h>
#include <Sddl.h>

// App launching
#include <atlbase.h>
#include <ShObjIdl.h>

const wchar_t* DLLFile = L"Osiris.dll";

const wchar_t* ApplicationUserModelID = L"Microsoft.Halo5Forge_8wekyb3d8bbwe!Ausar";
const wchar_t* PackageID = L"Microsoft.Halo5Forge_1.114.4592.2_x64__8wekyb3d8bbwe";

// UWP apps require DLLS with "ALL APPLICATION PACKAGES" group
void SetAccessControl(std::wstring ExecutableName);

uint32_t DLLInjectRemote(uint32_t ProcessID, const std::wstring& DLLpath);

bool LaunchAppUWP(const std::wstring &PackageName, uint32_t *ProcessID);

std::wstring GetRunningDirectory();

int main()
{
    void* hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleOutputCP(437);

    size_t ConsoleWidth = 80;
    CONSOLE_SCREEN_BUFFER_INFO ConsoleBuf;
    if( GetConsoleScreenBufferInfo(hStdout, &ConsoleBuf) )
    {
        ConsoleWidth = ConsoleBuf.dwSize.X;
    }

    SetConsoleTextAttribute(hStdout,
        FOREGROUND_RED |
        FOREGROUND_GREEN |
        FOREGROUND_INTENSITY);
    std::wcout << "Osiris Injector Build date (" << __DATE__ << " : " << __TIME__ << ")" << std::endl;
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_BLUE |
        FOREGROUND_GREEN |
        FOREGROUND_INTENSITY);
    std::wcout << "\t-https://github.com/Wunkolo/Osiris\n";
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_RED |
        FOREGROUND_BLUE);
    std::wcout << std::wstring(ConsoleWidth - 1, '-') << std::endl;
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_RED |
        FOREGROUND_GREEN |
        FOREGROUND_INTENSITY);

    uint32_t ProcessID = 0;

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    CComQIPtr<IPackageDebugSettings> DebugSettings;

    DebugSettings.CoCreateInstance(
        CLSID_PackageDebugSettings,
        nullptr,
        CLSCTX_ALL
    );

    DebugSettings->TerminateAllProcesses(PackageID);

    DebugSettings->EnableDebugging(
        PackageID,
        nullptr,
        nullptr
    );

    std::wcout << "Launching Halo 5: Forge...";

    LaunchAppUWP(ApplicationUserModelID, &ProcessID);
    std::wcout << "Launched" << std::endl;

    //DebugSettings->Suspend(PackageID);

    std::wstring CurrentDirectory = GetRunningDirectory();

    SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::wcout << "Injecting "
        << CurrentDirectory << "\\" << DLLFile
        << " into process ID " << ProcessID << ": ";

    if( DLLInjectRemote(ProcessID, CurrentDirectory + L"\\" + DLLFile) )
    {
        SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "Success!" << std::endl;
    }
    else
    {
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::cout << "Failed" << std::endl;
        system("pause");
    }

    DebugSettings->DisableDebugging(PackageID);
    DebugSettings.Release();
    CoUninitialize();

    return 0;
}

void SetAccessControl(std::wstring ExecutableName)
{
    PSECURITY_DESCRIPTOR *SecurityDescriptor = nullptr;
    EXPLICIT_ACCESSW ExplicitAccess = { 0 };

    ACL *AccessControlCurrent = nullptr;
    ACL *AccessControlNew = nullptr;

    SECURITY_INFORMATION SecurityInfo = DACL_SECURITY_INFORMATION;
    PSID SecurityIdentifier = nullptr;

    if( GetNamedSecurityInfoW(
        ExecutableName.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        &AccessControlCurrent,
        nullptr,
        SecurityDescriptor) == ERROR_SUCCESS )
    {
        ConvertStringSidToSidW(L"S-1-15-2-1", &SecurityIdentifier);
        if( SecurityIdentifier != nullptr )
        {
            ExplicitAccess.grfAccessPermissions = GENERIC_READ | GENERIC_EXECUTE;
            ExplicitAccess.grfAccessMode = SET_ACCESS;
            ExplicitAccess.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
            ExplicitAccess.Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ExplicitAccess.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
            ExplicitAccess.Trustee.ptstrName = reinterpret_cast<wchar_t*>(SecurityIdentifier);

            if( SetEntriesInAclW(
                1,
                &ExplicitAccess,
                AccessControlCurrent,
                &AccessControlNew) == ERROR_SUCCESS )
            {
                SetNamedSecurityInfoW(
                    const_cast<wchar_t*>(ExecutableName.c_str()),
                    SE_FILE_OBJECT,
                    SecurityInfo,
                    nullptr,
                    nullptr,
                    AccessControlNew,
                    nullptr);
            };
        }
    }
    if( SecurityDescriptor )
    {
        LocalFree(
            reinterpret_cast<HLOCAL>(SecurityDescriptor)
        );
    }
    if( AccessControlNew )
    {
        LocalFree(
            reinterpret_cast<HLOCAL>(AccessControlNew)
        );
    }
}

uint32_t DLLInjectRemote(uint32_t ProcessID, const std::wstring& DLLpath)
{
    const size_t DLLPathSize = ((DLLpath.size() + 1) * sizeof(wchar_t));
    uint32_t Result = 0;
    if( !ProcessID )
    {
        return false;
    }

    if( GetFileAttributesW(DLLpath.c_str()) == INVALID_FILE_ATTRIBUTES )
    {
        std::wcout << "DLL file: " << DLLpath << " does not exists" << std::endl;
        return false;
    }

    SetAccessControl(DLLpath);

    void* ProcLoadLibrary = reinterpret_cast<void*>(
        GetProcAddress(
            GetModuleHandleW(L"kernel32.dll"),
            "LoadLibraryW")
        );

    if( !ProcLoadLibrary )
    {
        std::wcout << "Unable to find LoadLibraryW procedure" << std::endl;
        return false;
    }

    void* Process = OpenProcess(
        PROCESS_ALL_ACCESS,
        false,
        ProcessID);
    if( Process == nullptr )
    {
        std::wcout << "Unable to open process for writing" << std::endl;
        return false;
    }
    void* VirtualAlloc = reinterpret_cast<void*>(
        VirtualAllocEx(
            Process,
            nullptr,
            DLLPathSize,
            MEM_RESERVE | MEM_COMMIT,
            PAGE_READWRITE
        )
        );

    if( VirtualAlloc == nullptr )
    {
        std::wcout << "Unable to remotely allocate memory" << std::endl;
        CloseHandle(Process);
        return false;
    }

    size_t BytesWritten = 0;
    Result = WriteProcessMemory(
        Process,
        VirtualAlloc,
        DLLpath.data(),
        DLLPathSize,
        &BytesWritten
    );

    if( Result == 0 )
    {
        std::wcout << "Unable to write process memory" << std::endl;
        CloseHandle(Process);
        return false;
    }

    if( BytesWritten != DLLPathSize )
    {
        std::wcout << "Failed to write remote DLL path name" << std::endl;
        CloseHandle(Process);
        return false;
    }

    void* RemoteThread =
        CreateRemoteThread(
            Process,
            nullptr,
            0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(ProcLoadLibrary),
            VirtualAlloc,
            0,
            0);

    // Wait for remote thread to finish
    if( RemoteThread )
    {
        // Explicitly wait for LoadLibraryW to complete before releasing memory
        // avoids causing a remote memory leak
        WaitForSingleObject(RemoteThread, INFINITE);
        CloseHandle(RemoteThread);
    }
    else
    {
        // Failed to create thread
        std::wcout << "Unable to create remote thread" << std::endl;
    }

    VirtualFreeEx(Process, VirtualAlloc, 0, MEM_RELEASE);
    CloseHandle(Process);
    return Result;
}

bool LaunchAppUWP(const std::wstring &PackageID, uint32_t *ProcessID)
{
    CComPtr<IApplicationActivationManager> ApplicationManager;

    if(
        CoCreateInstance(
            CLSID_ApplicationActivationManager,
            nullptr,
            CLSCTX_LOCAL_SERVER,
            IID_IApplicationActivationManager,
            (LPVOID*)&ApplicationManager
        ) == S_OK
        )
    {
        CoAllowSetForegroundWindow(
            ApplicationManager,
            nullptr
        );
        // Launch
        ApplicationManager->ActivateApplication(
            PackageID.c_str(),
            nullptr,
            AO_NONE,
            reinterpret_cast<DWORD*>(ProcessID)
        );
        return true;
    }
    return false;
}

std::wstring GetRunningDirectory()
{
	wchar_t exePath[MAX_PATH];
	GetModuleFileNameW(GetModuleHandle(NULL), exePath, (sizeof(exePath)));
	PathRemoveFileSpecW(exePath);
	return std::wstring(exePath);
}