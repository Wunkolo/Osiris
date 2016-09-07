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

const char* DLLFile = "Thoth.dll";

const char* ApplicationUserModelID = "Microsoft.Halo5Forge_8wekyb3d8bbwe!Ausar";
const char* PackageID = "Microsoft.Halo5Forge_1.114.4592.2_x64__8wekyb3d8bbwe";

// UWP apps require DLLS with "ALL APPLICATION PACKAGES" group
void SetAccessControl(std::string ExecutableName);

bool DLLInjectRemote(uint32_t ProcessID, const std::string& DLLpath);

bool LaunchAppUWP(std::string &PackageName, uint32_t *ProcessID);

int main()
{
    uint32_t ProcessID = 0;

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    LaunchAppUWP(std::string(ApplicationUserModelID), &ProcessID);
    CoUninitialize();

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
    std::cout << "Thoth Injector Build date (" << __DATE__ << " : " << __TIME__ << ")" << std::endl;
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_BLUE |
        FOREGROUND_GREEN |
        FOREGROUND_INTENSITY);
    std::cout << "\t-Wunkolo (Wunkolo@gmail.com)\n";
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_RED |
        FOREGROUND_BLUE);
    std::cout << std::string(ConsoleWidth - 1, '-') << std::endl;
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_RED |
        FOREGROUND_GREEN |
        FOREGROUND_INTENSITY);

    std::string CurrentDirectory(MAX_PATH, 0);
    CurrentDirectory.resize(GetCurrentDirectoryA(MAX_PATH, &CurrentDirectory[0]));

    SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "Injecting "
        << CurrentDirectory << "\\" << DLLFile
        << " into process ID " << ProcessID << ": ";

    if( DLLInjectRemote(ProcessID, CurrentDirectory + "\\" + DLLFile) )
    {
        SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "Success!" << std::endl;
        return 0;
    }
    else
    {
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::cout << "Failed" << std::endl;
    }
    system("pause");
    return 0;
}

void SetAccessControl(std::string ExecutableName)
{
    PSECURITY_DESCRIPTOR SecurityDescriptor = nullptr;
    EXPLICIT_ACCESS ExplicitAccess = { 0 };

    PACL AccessControlCurrent = nullptr;
    PACL AccessControlNew = nullptr;

    SECURITY_INFORMATION SecurityInfo = DACL_SECURITY_INFORMATION;
    PSID SecurityIdentifier;

    if( GetNamedSecurityInfoA(
        ExecutableName.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        &AccessControlCurrent,
        nullptr,
        &SecurityDescriptor) == ERROR_SUCCESS )
    {
        ConvertStringSidToSidA("S-1-15-2-1", &SecurityIdentifier);
        if( SecurityIdentifier != nullptr )
        {
            ExplicitAccess.grfAccessPermissions = GENERIC_READ | GENERIC_EXECUTE;
            ExplicitAccess.grfAccessMode = SET_ACCESS;
            ExplicitAccess.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
            ExplicitAccess.Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ExplicitAccess.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
            ExplicitAccess.Trustee.ptstrName = reinterpret_cast<char*>(SecurityIdentifier);

            if( SetEntriesInAclA(
                1,
                &ExplicitAccess,
                AccessControlCurrent,
                &AccessControlNew) == ERROR_SUCCESS )
            {
                SetNamedSecurityInfoA(
                    const_cast<char*>(ExecutableName.c_str()),
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

bool DLLInjectRemote(uint32_t ProcessID, const std::string& DLLpath)
{
    if( !ProcessID )
    {
        return false;
    }

    if( GetFileAttributes(DLLpath.c_str()) == INVALID_FILE_ATTRIBUTES )
    {
        std::cout << "DLL file: " << DLLpath << " does not exists" << std::endl;
        return false;
    }

    SetAccessControl(DLLpath);

    void* ProcLoadLibrary = reinterpret_cast<void*>(GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA"));
    if( !ProcLoadLibrary )
    {
        return false;
    }

    void* Process = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE |
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        false,
        ProcessID);
    if( !Process )
    {
        return false;
    }
    void* Alloc = reinterpret_cast<void*>(VirtualAllocEx(
        Process,
        nullptr,
        DLLpath.length() + 1,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE));
    if( !Alloc )
    {
        return false;
    }
    WriteProcessMemory(Process, Alloc, DLLpath.c_str(), DLLpath.length() + 1, nullptr);

    void* RemoteThread =
        CreateRemoteThread(
            Process,
            nullptr,
            0,
            (LPTHREAD_START_ROUTINE)ProcLoadLibrary,
            Alloc,
            0,
            0);

    bool Result = false;

    // Wait for remote thread to finish
    if( RemoteThread )
    {
        Result = WaitForSingleObject(RemoteThread, 10000) != WAIT_TIMEOUT;
    }

    VirtualFreeEx(Process, Alloc, 0, MEM_RELEASE);
    CloseHandle(Process);
    return Result;
}

bool LaunchAppUWP(std::string &PackageName, uint32_t *ProcessID)
{
    CComPtr<IApplicationActivationManager> ApplicationManager;

    std::wstring_convert < std::codecvt_utf8_utf16<wchar_t>> Widener;

    std::wstring WidePackageName = Widener.from_bytes(PackageName);

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
            WidePackageName.c_str(),
            nullptr,
            AO_NONE,
            reinterpret_cast<DWORD*>(ProcessID)
        );
        return true;
    }
    return false;
}