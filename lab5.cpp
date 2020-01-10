#include <windows.h>
#include <iostream>
#include <w32api/tlhelp32.h>

#define THREADS_COUNT 3

using namespace std;

typedef struct {
    int ascending;
    char charToFind;
    int resultPosition;
} params;

static const char *const fileName = "array.txt";

char *sortArray(char *array, int size, const int *asc) {
    //сортировка методом выбора

    for (int i = 0; i < size - 1; i++) {
        char fst = array[i];
        for (int y = i + 1; y < size; y++) {
            if ((asc && fst > array[y]) || (!asc && fst < array[y])) {
                fst = array[y];
                array[y] = array[i];
                array[i] = fst;
            }
        }
    }
    return array;
}

DWORD WINAPI Proc(CONST LPVOID lpParam) {
    SYSTEMTIME startTime, finishTime;
    GetSystemTime(&startTime);

    auto *param = (params *) lpParam;

    HANDLE hFile = CreateFile(fileName,
                              GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr,
                              OPEN_EXISTING,
                              0,
                              nullptr);
    HANDLE hMap = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, 0, nullptr);
    DWORD dwFileSize = GetFileSize(hFile, nullptr);

    /*Выделение памяти и считывание данных*/
    auto *memory = (char *) MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, dwFileSize);
    int dataSize = 1;

    /*Подготовка массива*/
    auto *data = new char[dwFileSize / dataSize];
    for (int i = 0; i <= dwFileSize / dataSize; i++)
        data[i] = *(memory + i * dataSize);

    /*Сортировка*/
    data = sortArray(data, dwFileSize, &param->ascending);

    /*Запись в файл*/
    for (int i = 0; i <= dwFileSize / dataSize; i++)
        memory[i] = *(data + i * dataSize);

    char value = param->charToFind;

    /* поиск */
    for (int i = 0; i < dwFileSize; i++) {
        if (memory[i] == value) {
            param->resultPosition = i;
            break;
        }
    }

    UnmapViewOfFile(memory);

    CloseHandle(hMap);
    CloseHandle(hFile);

    GetSystemTime(&finishTime);
    cout << "thread id: " << GetCurrentThreadId();
    cout << "   время:" << finishTime.wMilliseconds - startTime.wMilliseconds << "мс." << endl;
    ExitThread(0);
}

int main() {
    params params1;
    cout << "Сортировка:" << endl;
    cout << "1 - по возрастанию" << endl;
    cout << "2 - по убыванию" << endl;
    cin >> params1.ascending;
    cout << "Символ для поиска:" << endl;
    cin >> params1.charToFind;

    HANDLE hTHREADS[THREADS_COUNT];

    for (auto &i : hTHREADS) {
        i = CreateThread(nullptr, 0, &Proc, &params1, 0, nullptr);
        if (nullptr == i) {
            cout << "Не удалось создать поток" << endl;
            return 1;
        }
        Sleep(5);
    }

    WaitForMultipleObjects(THREADS_COUNT, hTHREADS, TRUE, INFINITE);

    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    int count = 0;
    THREADENTRY32 te;
    if (h != INVALID_HANDLE_VALUE)
    {
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te))
        {
            do
            {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
                    count++;
                te.dwSize = sizeof(te);
            } while (Thread32Next(h, &te));
        }
        CloseHandle(h);
    }

    std::cout << "Количество открытых дескрипторов в системе - " << count << "\n";

    for (auto &i : hTHREADS) {
        CloseHandle(i);
    }

    cout << "Символ \"" << params1.charToFind << "\" найден по номеру " << params1.resultPosition << endl;
}