#include <windows.h>
#include <iostream>
#include <tlhelp32.h>

using namespace std;

PROCESS_INFORMATION startProcess(TCHAR commandLine[]) {
    STARTUPINFO StartupInfo;
    PROCESS_INFORMATION ProcInfo;
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);
    ZeroMemory(&ProcInfo, sizeof(ProcInfo));

    if (!CreateProcess(nullptr, // Не используется имя модуля
                       commandLine,          // Командная строка
                       nullptr,                 // Дескриптор процесса не наследуется.
                       nullptr,                 // Дескриптор потока не наследуется.
                       FALSE,                // Установка описателей наследования
                       0,                    // Нет флагов создания процесса
                       nullptr,                 // Блок переменных окружения родительского процесса
                       nullptr,                 // Использовать текущий каталог родительского процесса
                       &StartupInfo,         // Указатель на структуру  STARTUPINFO.
                       &ProcInfo)           // Указатель на структуру информации о процессе.
            )
        printf("CreateProcess failed.\n");
    return ProcInfo;
}

int main(int argc, char *argv[]) {
    std::cout << "lab4 process id - " << GetCurrentProcessId() << "\n";

    PROCESS_INFORMATION ProcInfo = startProcess(const_cast<TCHAR *>(TEXT("lab4_proc.exe 13 16384.txt")));
    Sleep(1000);
    PROCESS_INFORMATION ProcInfo2 = startProcess(const_cast<TCHAR *>(TEXT("lab4_proc.exe 25 4096.txt")));
    Sleep(1000);
    PROCESS_INFORMATION ProcInfo3 = startProcess(const_cast<TCHAR *>(TEXT("lab4_proc.exe 46 8192.txt")));
    Sleep(1000);

    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    int count = 0;
    THREADENTRY32 te;
    if (h != INVALID_HANDLE_VALUE) {
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te)) {
            do {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
                    count++;
                te.dwSize = sizeof(te);
            } while (Thread32Next(h, &te));
        }
        CloseHandle(h);
    }

    std::cout << "Количество открытых дескрипторов в системе - " << count << "\n";

    WaitForSingleObject(ProcInfo.hProcess, INFINITE);
    CloseHandle(ProcInfo.hProcess);
    CloseHandle(ProcInfo.hThread);

    WaitForSingleObject(ProcInfo2.hProcess, INFINITE);
    CloseHandle(ProcInfo2.hProcess);
    CloseHandle(ProcInfo2.hThread);

    WaitForSingleObject(ProcInfo3.hProcess, INFINITE);
    CloseHandle(ProcInfo3.hProcess);
    CloseHandle(ProcInfo3.hThread);

    return 0;
}