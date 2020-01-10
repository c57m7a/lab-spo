#include "windows.h"
#include <iostream>
#include <fstream>

using namespace std;

void criticalSections();

void mutex();

void semaphore();

void events();

void fileRead(const char *str);

void fileWrite(const char *fileName);

void mergeSort(int *a, long lb, long ub);

void merge(int a[], long lb, long split, long ub);

void printResult();

DWORD WINAPI arraySearchCS(void *);

DWORD WINAPI simpleArraySortCS(void *);

DWORD WINAPI naturalArraySortCS(void *);

DWORD WINAPI arraySearchM(void *);

DWORD WINAPI simpleArraySortM(void *);

DWORD WINAPI naturalArraySortM(void *);

DWORD WINAPI arraySearchS(void *);

DWORD WINAPI simpleArraySortS(void *);

DWORD WINAPI naturalArraySortS(void *);

DWORD WINAPI arraySearchE(void *);

DWORD WINAPI simpleArraySortE(void *);

DWORD WINAPI naturalArraySortE(void *);

HANDLE hEvent, hMutex, hSemaphore; // дескрипторы события, мютекса и семафора
CRITICAL_SECTION critical_section; // указатель на критическую секцию

int symbolToSearch = 9;
const int THREADS_COUNT = 3;
const int ARRAY_SIZE = 16380;
static const char *const FILE_NAME = "arrayc.txt";
int array[ARRAY_SIZE];
long duration[4];

int main() {
    setlocale(LC_ALL, "Russian");

    InitializeCriticalSection(&critical_section); // Инициализация критической секции
    long startTime, finishTime;
    startTime = clock();
    criticalSections();
    finishTime = clock();
    duration[0] = finishTime - startTime;

    Sleep(2000);
    cout << endl;

    hMutex = CreateMutex(nullptr, FALSE, "Mutex1");
    startTime = clock();
    mutex();
    finishTime = clock();
    duration[1] = finishTime - startTime;

    Sleep(2000);
    cout << endl;

    hSemaphore = CreateSemaphore(nullptr, 1, 1, nullptr); //Инициализация семафора

    startTime = clock();
    semaphore();
    finishTime = clock();
    duration[2] = finishTime - startTime;

    Sleep(3000);
    cout << endl;

    hEvent = CreateEvent(nullptr, TRUE, FALSE, "Event");

    startTime = clock();
    events();
    finishTime = clock();
    duration[3] = finishTime - startTime;

    Sleep(2000);
    cout << endl;

    printResult();
}

void fileRead(const char *str) {
    setlocale(LC_ALL, "RUSSIAN");
    ifstream in(str);

    if (in.is_open()) {
        int temp;

        int i = 0;
        while (!in.eof()) {
            in >> temp;
            array[i] = temp;
            i++;
        }

        in.close();
    } else cout << "Файл не найден.";
}

void fileWrite(const char *fileName) {
    fstream stream;

    stream.open(fileName, ios::out);
    for (int j = 0; j < ARRAY_SIZE; j++) {
        if ((j != 0) && (j % 10 == 0)) stream << "\n";
        stream << array[j] << "\t";
    }
    stream.close();
}

void mergeSort(int *a, long lb, long ub) {
    long split;

    if (lb < ub) {
        split = (lb + ub) / 2;

        mergeSort(a, lb, split);
        mergeSort(a, split + 1, ub);
        merge(a, lb, split, ub);
    }
}

void merge(int a[], long lb, long split, long ub) {
    long left = lb;
    long mid = split + 1;
    long right = 0;

    auto *temp = new int[ub - lb + 1];

    while (left <= split && mid <= ub) {
        if (a[left] < a[mid])
            temp[right++] = a[left++];
        else
            temp[right++] = a[mid++];
    }

    while (mid <= ub)
        temp[right++] = a[mid++];
    while (left <= split)
        temp[right++] = a[left++];

    for (right = 0; right < ub - lb + 1; right++)
        a[lb + right] = temp[right];

}

void printResult() {
    cout << "Критические секции " << duration[0] << " мс." << endl;
    cout << "Мьютекс " << duration[1] << " мс." << endl;
    cout << "Семафор " << duration[2] << " мс." << endl;
    cout << "События " << duration[3] << " мс." << endl;
}

void criticalSections() {
    fileRead(FILE_NAME);

    cout << "Критические секции:\n";

    HANDLE hTHREADS[THREADS_COUNT];

    hTHREADS[0] = CreateThread(nullptr, 0, simpleArraySortCS, nullptr, 0, nullptr);
    hTHREADS[1] = CreateThread(nullptr, 0, naturalArraySortCS, nullptr, 0, nullptr);
    hTHREADS[2] = CreateThread(nullptr, 0, arraySearchCS, nullptr, 0, nullptr);

    if ((hTHREADS[0] == nullptr) || (hTHREADS[1] == nullptr) || (hTHREADS[2] == nullptr)) { return; }

    WaitForMultipleObjects(NULL, hTHREADS, TRUE, INFINITE);

    fileWrite("1.1");
    for (auto &i : hTHREADS) {
        CloseHandle(i);
    }
}

DWORD WINAPI arraySearchCS(void *) {
    DWORD id = GetCurrentThreadId();

    EnterCriticalSection(&critical_section);
    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;
    ifstream in(FILE_NAME);

    if (in.is_open()) {
        int temp;

        int i = 0;
        while (!in.eof()) {
            in >> temp;
            if (temp == symbolToSearch) break;
            i++;
        }
        in.close();
    } else cout << "Файл не найден.";
    cout << "\nЗавершен поток с id - " << id;
    LeaveCriticalSection(&critical_section);
    ExitThread(0);
}

DWORD WINAPI simpleArraySortCS(void *) {
    DWORD id = GetCurrentThreadId();
    EnterCriticalSection(&critical_section);

    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;
    int workArray[ARRAY_SIZE];
    for (int j = 0; j < ARRAY_SIZE; j++) workArray[j] = array[j];
    LeaveCriticalSection(&critical_section);

    mergeSort(workArray, 0, ARRAY_SIZE - 1);

    EnterCriticalSection(&critical_section);

    cout << "\n\tПолучен доступ к данным для потока " << id;
    for (int j = 0; j < ARRAY_SIZE; j++) array[j] = workArray[j];
    cout << "\nЗавершен поток с id - " << id;
    LeaveCriticalSection(&critical_section);

    ExitThread(0);
}

DWORD WINAPI naturalArraySortCS(void *) {
    DWORD id = GetCurrentThreadId();
    EnterCriticalSection(&critical_section);

    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;
    int workArray[ARRAY_SIZE];
    for (int j = 0; j < ARRAY_SIZE; j++) workArray[j] = array[j];
    LeaveCriticalSection(&critical_section);

    mergeSort(workArray, 0, ARRAY_SIZE - 1);

    EnterCriticalSection(&critical_section);

    cout << "\n\tПолучен доступ к данным для потока " << id;
    for (int j = 0; j < ARRAY_SIZE; j++) array[j] = workArray[j];
    cout << "\nЗавершен поток с id - " << id;
    LeaveCriticalSection(&critical_section);
    ExitThread(0);
}

void mutex() {
    fileRead(FILE_NAME);

    cout << "Мьютекс" << endl;

    HANDLE hTHREADS[THREADS_COUNT];

    hTHREADS[0] = CreateThread(nullptr, 0, simpleArraySortM, nullptr, 0, nullptr);
    hTHREADS[1] = CreateThread(nullptr, 0, naturalArraySortM, nullptr, 0, nullptr);
    hTHREADS[2] = CreateThread(nullptr, 0, arraySearchM, nullptr, 0, nullptr);

    if ((hTHREADS[0] == nullptr) || (hTHREADS[1] == nullptr) || (hTHREADS[2] == nullptr)) { return; }

    WaitForMultipleObjects(NULL, hTHREADS, TRUE, INFINITE);

    fileWrite("2.1");
    for (auto &i : hTHREADS) {
        CloseHandle(i);
    }
    CloseHandle(hMutex);
}

DWORD WINAPI arraySearchM(void *) {
    DWORD id = GetCurrentThreadId();

    WaitForSingleObject(hMutex, INFINITE);
    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;
    ifstream in(FILE_NAME);

    if (in.is_open()) {
        int temp;

        int i = 0;
        while (!in.eof()) {
            in >> temp;
            if (temp == symbolToSearch) break;
            i++;
        }
        in.close();
    } else cout << "Файл не найден.";

    cout << "\nЗавершен поток с id - " << id;
    ReleaseMutex(hMutex);
    ExitThread(0);
}

DWORD WINAPI simpleArraySortM(void *) {
    DWORD id = GetCurrentThreadId();

    WaitForSingleObject(hMutex, INFINITE);
    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;
    int workArray[ARRAY_SIZE];
    for (int j = 0; j < ARRAY_SIZE; j++) workArray[j] = array[j];
    ReleaseMutex(hMutex);

    mergeSort(workArray, 0, ARRAY_SIZE - 1);

    WaitForSingleObject(hMutex, INFINITE);
    cout << "\n\tПолучен доступ к данным для потока " << id;
    for (int j = 0; j < ARRAY_SIZE; j++) array[j] = workArray[j];
    ReleaseMutex(hMutex);

    cout << "\nЗавершен поток с id - " << id;
    ExitThread(0);
}

DWORD WINAPI naturalArraySortM(void *) {
    DWORD id = GetCurrentThreadId();

    WaitForSingleObject(hMutex, INFINITE);
    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;
    int workArray[ARRAY_SIZE];
    for (int j = 0; j < ARRAY_SIZE; j++) workArray[j] = array[j];
    ReleaseMutex(hMutex);

    mergeSort(workArray, 0, ARRAY_SIZE - 1);

    WaitForSingleObject(hMutex, INFINITE);
    cout << "\n\tПолучен доступ к данным для потока " << id;
    for (int j = 0; j < ARRAY_SIZE; j++) array[j] = workArray[j];
    ReleaseMutex(hMutex);

    cout << "\nЗавершен поток с id - " << id;
    ExitThread(0);
}

void semaphore() {
    fileRead(FILE_NAME);

    cout << "Семафор:" << endl;

    HANDLE hTHREADS[THREADS_COUNT];

    hTHREADS[0] = CreateThread(nullptr, 0, simpleArraySortS, nullptr, 0, nullptr);
    hTHREADS[1] = CreateThread(nullptr, 0, naturalArraySortS, nullptr, 0, nullptr);
    hTHREADS[2] = CreateThread(nullptr, 0, arraySearchS, nullptr, 0, nullptr);

    if ((hTHREADS[0] == nullptr) || (hTHREADS[1] == nullptr) || (hTHREADS[2] == nullptr)) { return; }

    WaitForMultipleObjects(NULL, hTHREADS, TRUE, INFINITE);

    fileWrite("3.1");
    for (auto &i : hTHREADS) {
        CloseHandle(i);
    }

    CloseHandle(hSemaphore);
}

DWORD WINAPI arraySearchS(void *) {
    DWORD id = GetCurrentThreadId();

    WaitForSingleObject(hSemaphore, INFINITE);
    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;
    ifstream in(FILE_NAME);

    if (in.is_open()) {
        int temp;

        int i = 0;
        while (!in.eof()) {
            in >> temp;
            if (temp == symbolToSearch) break;
            i++;
        }
        in.close();
    } else cout << "Файл не найден.";

    cout << "\nЗавершен поток с id - " << id;
    ReleaseSemaphore(hSemaphore, 1, nullptr);
    ExitThread(0);
}

DWORD WINAPI simpleArraySortS(void *) {
    DWORD id = GetCurrentThreadId();

    WaitForSingleObject(hSemaphore, INFINITE);
    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;
    int workArray[ARRAY_SIZE];
    for (int j = 0; j < ARRAY_SIZE; j++) workArray[j] = array[j];
    ReleaseSemaphore(hSemaphore, 1, nullptr);

    mergeSort(workArray, 0, ARRAY_SIZE - 1);

    WaitForSingleObject(hSemaphore, INFINITE);
    cout << "\n\tПолучен доступ к данным для потока " << id;
    for (int j = 0; j < ARRAY_SIZE; j++) array[j] = workArray[j];
    ReleaseSemaphore(hSemaphore, 1, nullptr);

    cout << "\nЗавершен поток с id - " << id;
    ExitThread(0);
}

DWORD WINAPI naturalArraySortS(void *) {
    DWORD id = GetCurrentThreadId();

    WaitForSingleObject(hSemaphore, INFINITE);

    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;
    int workArray[ARRAY_SIZE];
    for (int j = 0; j < ARRAY_SIZE; j++) workArray[j] = array[j];
    ReleaseSemaphore(hSemaphore, 1, nullptr);

    mergeSort(workArray, 0, ARRAY_SIZE - 1);

    WaitForSingleObject(hSemaphore, INFINITE);
    cout << "\n\tПолучен доступ к данным для потока " << id;
    for (int j = 0; j < ARRAY_SIZE; j++) array[j] = workArray[j];
    ReleaseSemaphore(hSemaphore, 1, nullptr);

    cout << "\nЗавершен поток с id - " << id;
    ExitThread(0);
}

void events() {
    fileRead(FILE_NAME);

    cout << "События:" << endl;

    HANDLE hTHREADS[THREADS_COUNT];

    hTHREADS[0] = CreateThread(nullptr, 0, simpleArraySortE, nullptr, 0, nullptr);
    hTHREADS[1] = CreateThread(nullptr, 0, naturalArraySortE, nullptr, 0, nullptr);
    hTHREADS[2] = CreateThread(nullptr, 0, arraySearchE, nullptr, 0, nullptr);
    SetEvent(hEvent);
    if ((hTHREADS[0] == nullptr) || (hTHREADS[1] == nullptr) || (hTHREADS[2] == nullptr)) { return; }

    WaitForMultipleObjects(NULL, hTHREADS, TRUE, INFINITE);

    fileWrite("4.1");

    for (auto &i : hTHREADS) {
        CloseHandle(i);
    }

    CloseHandle(hEvent);
}

DWORD WINAPI arraySearchE(void *) {
    DWORD id = GetCurrentThreadId();

    WaitForSingleObject(hEvent, INFINITE);
    ResetEvent(hEvent);

    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;
    ifstream in(FILE_NAME);

    if (in.is_open()) {
        int temp;

        int i = 0;
        while (!in.eof()) {
            in >> temp;
            if (temp == symbolToSearch) break;
            i++;
        }
        in.close();
    } else cout << "Файл не найден.";

    cout << "\nЗавершен поток с id - " << id;
    SetEvent(hEvent);
    ExitThread(0);
}

DWORD WINAPI simpleArraySortE(void *) {
    DWORD id = GetCurrentThreadId();

    WaitForSingleObject(hEvent, INFINITE);
    ResetEvent(hEvent);

    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;

    int workArray[ARRAY_SIZE];
    for (int j = 0; j < ARRAY_SIZE; j++) workArray[j] = array[j];
    SetEvent(hEvent);

    mergeSort(workArray, 0, ARRAY_SIZE - 1);

    WaitForSingleObject(hEvent, INFINITE);

    ResetEvent(hEvent);
    cout << "\n\tПолучен доступ к данным для потока " << id;

    for (int j = 0; j < ARRAY_SIZE; j++) array[j] = workArray[j];

    cout << "\nЗавершен поток с id - " << id;
    SetEvent(hEvent);
    ExitThread(0);
}

DWORD WINAPI naturalArraySortE(void *) {
    DWORD id = GetCurrentThreadId();

    WaitForSingleObject(hEvent, INFINITE);
    ResetEvent(hEvent);  

    cout << "\nЗапущен поток с id - " << id;
    cout << "\n\tПолучен доступ к данным для потока " << id;

    int workArray[ARRAY_SIZE];
    for (int j = 0; j < ARRAY_SIZE; j++) workArray[j] = array[j];
    SetEvent(hEvent); 
    
    mergeSort(workArray, 0, ARRAY_SIZE - 1);

    WaitForSingleObject(hEvent, INFINITE);
    ResetEvent(hEvent);  
    cout << "\n\tПолучен доступ к данным для потока " << id;

    for (int j = 0; j < ARRAY_SIZE; j++) array[j] = workArray[j];

    cout << "\nЗавершен поток с id - " << id;
    SetEvent(hEvent);
    ExitThread(0);
}