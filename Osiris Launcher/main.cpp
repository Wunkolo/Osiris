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

bool LaunchAppUWP(std::wstring &PackageName, uint32_t *ProcessID);

int main()
{
    uint32_t ProcessID = 0;

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    LaunchAppUWP(ApplicationUserModelID, &ProcessID);
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
    std::wcout << "Osiris Injector Build date (" << __DATE__ << " : " << __TIME__ << ")" << std::endl;
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_BLUE |
        FOREGROUND_GREEN |
        FOREGROUND_INTENSITY);
    std::wcout << "\t-Wunkolo (Wunkolo@gmail.com)\n";
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_RED |
        FOREGROUND_BLUE);
    std::wcout << std::wstring(ConsoleWidth - 1, '-') << std::endl;
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_RED |
        FOREGROUND_GREEN |
        FOREGROUND_INTENSITY);

    std::wstring CurrentDirectory(MAX_PATH, 0);
    CurrentDirectory.resize(GetCurrentDirectoryW(MAX_PATH, &CurrentDirectory[0]));

    SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::wcout << "Injecting "
        << CurrentDirectory << "\\" << DLLFile
        << " into process ID " << ProcessID << ": ";

    if( DLLInjectRemote(ProcessID, CurrentDirectory + L"\\" + DLLFile) )
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

void SetAccessControl(std::wstring ExecutableName)
{
    PSECURITY_DESCRIPTOR SecurityDescriptor = nullptr;
    EXPLICIT_ACCESS ExplicitAccess = { 0 };

    PACL AccessControlCurrent = nullptr;
    PACL AccessControlNew = nullptr;

    SECURITY_INFORMATION SecurityInfo = DACL_SECURITY_INFORMATION;
    PSID SecurityIdentifier;

    if( GetNamedSecurityInfoW(
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

    void* ProcLoadLibrary = reinterpret_cast<void*>(GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA"));
    if( !ProcLoadLibrary )
    {
        std::wcout << "Unable to find LoadLibraryA procedure" << std::endl;
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
        std::wcout << "Unable to open process for writing" << std::endl;
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
        std::wcout << "Unable to remotely allocate memory" << std::endl;
        return false;
    }
    Result = WriteProcessMemory(Process, Alloc, DLLpath.c_str(), DLLpath.length() + 1, nullptr);

    if( Result == 0 )
    {
        std::wcout << "Unable to write process memory" << std::endl;
    }

    void* RemoteThread =
        CreateRemoteThread(
            Process,
            nullptr,
            0,
            (LPTHREAD_START_ROUTINE)ProcLoadLibrary,
            Alloc,
            0,
            0);

    // Wait for remote thread to finish
    if( RemoteThread )
    {
        WaitForSingleObject(RemoteThread, 10000);
    }
    else
    {
        // Failed to create thread
        std::cout << "Unable to create remote thread" << std::endl;
        return 0;
    }

    VirtualFreeEx(Process, Alloc, 0, MEM_RELEASE);
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