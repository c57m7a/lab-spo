#include <iostream>
#include <windows.h>
#include <fstream>

void sortArray(int *array, int size) {
    //сортировка методом выбора

    for (int i = 0; i < size - 1; i++) {
        int min = array[i];
        for (int y = i + 1; y < size; y++) {
            if (min > array[y]) {
                min = array[y];
                array[y] = array[i];
                array[i ] = min;
            }
        }
    }
}

int findInArray(int *array, int key, int size) {
    //бинарный поиск

    int left, right, result = -1;
    left = 0;
    right = size - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        if (key == array[mid]) {
            result = mid;
            break;
        }
        if (key < array[mid])
            right = mid - 1;
        else
            left = mid + 1;
    }
    if (result == -1)
        std::cout << "Element not found\n";
    else;
//        std::cout << "Element: " << array[result] << "\n";
}

void printHeading() {
    int i;

    std::cout << "╔";
    for (i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╦";
    for (i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╦";
    for (i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╦";
    for (i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╗" << std::endl;

    std::cout << "║";
    printf("  Numbers count  ");
    std::cout << "║";
    printf("    Heap, ms     ");
    std::cout << "║";
    printf("     MMF, ms     ");
    std::cout << "║";
    printf("Base indexing, ms");
    std::cout << "║" << std::endl;

    std::cout << "╠";
    for (i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╬";
    for (i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╬";
    for (i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╬";
    for (i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╣" << std::endl;
}

void printLine(int size, int c, int windows, int copyFile) {
    std::cout << "║";
    printf("%17d", size);
    std::cout << "║";
    printf("%17d", c);
    std::cout << "║";
    printf("%17d", windows);
    std::cout << "║";
    printf("%17d", copyFile);
    std::cout << "║" << std::endl;
}

void printSeparator() {
    std::cout << "╠";
    for (int i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╬";
    for (int i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╬";
    for (int i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╬";
    for (int i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╣" << std::endl;
}

void printFooter() {
    std::cout << "╚";
    for (int i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╩";
    for (int i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╩";
    for (int i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╩";
    for (int i = 0; i < 17; i++) {
        std::cout << "═";
    }
    std::cout << "╝" << std::endl;
}


int basePointers(char *file, int count) {
    auto hFile = CreateFile(file, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "fileMappingCreate - CreateFile failed, fname = " << file << std::endl;
        return -1;
    }

    HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (hMapping == nullptr) {
        std::cerr << "fileMappingCreate - CreateFileMapping failed, fname = " << file << std::endl;
        CloseHandle(hFile);
        return -2;
    }

    DWORD dwFileSize = GetFileSize(hFile, nullptr);
    if (dwFileSize == INVALID_FILE_SIZE) {
        std::cerr << "fileMappingCreate - GetFileSize failed, fname = " << file << std::endl;
        CloseHandle(hFile);
        return -3;
    }

    auto *dataPtr = (char *) MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
    if (dataPtr == nullptr) {
        std::cerr << "fileMappingCreate - MapViewOfFile failed, fname = " << file << std::endl;
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return -4;
    }
    auto startTime = static_cast<int>(clock());

    char array1[dwFileSize];

    for (int i = 0; i < dwFileSize; i++) {
        array1[i] = *(dataPtr + i);
    }

    char *pEnd;
    long int t2;
    int array[count];
    long int t = strtol(dataPtr, &pEnd, 10);
    array[0] = static_cast<int>(t);
    for (int i = 1; i < count; i++) {
        t2 = strtol(pEnd, &pEnd, 10);
        array[i] = static_cast<int>(t2);
    }

    sortArray(array, count);
    findInArray(array, 4, count);

    UnmapViewOfFile(dataPtr);
    CloseHandle(hMapping);
    CloseHandle(hFile);

    auto endTime = static_cast<int>(clock());

    return endTime - startTime;
}

int mappingFiles(char *file, int count) {
    auto startTime = static_cast<int>(clock());

    auto hFile = CreateFile(file, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "fileMappingCreate - CreateFile failed, fname = " << file << std::endl;
        return -1;
    }

    auto hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (hMapping == nullptr) {
        std::cerr << "fileMappingCreate - CreateFileMapping failed, fname = " << file << std::endl;
        CloseHandle(hFile);
        return -2;
    }

    auto dwFileSize = GetFileSize(hFile, nullptr);
    if (dwFileSize == INVALID_FILE_SIZE) {
        std::cerr << "fileMappingCreate - GetFileSize failed, fname = " << file << std::endl;
        CloseHandle(hFile);
        return -3;
    }

    auto dataPtr = (char *) MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
    if (dataPtr == nullptr) {
        std::cerr << "fileMappingCreate - MapViewOfFile failed, fname = " << file << std::endl;
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return -4;
    }

    char *pEnd;
    int array[count];
    long int t = strtol(dataPtr, &pEnd, 10);
    array[0] = static_cast<int>(t);
    for (int i = 1; i < count; i++) {
        long int t2 = strtol(pEnd, &pEnd, 10);
        array[i] = static_cast<int>(t2);
    }

    sortArray(array, count);
    findInArray(array, 4, count);

    UnmapViewOfFile(dataPtr);
    CloseHandle(hMapping);
    CloseHandle(hFile);

    auto endTime = static_cast<int>(clock());
    return endTime - startTime;
}

int heap1(char *file, int count) {
    std::ifstream in3(file);

    if (in3.is_open()) {
        count = 0;
        int temp;

        while (!in3.eof()) {
            in3 >> temp;
            count++;
        }
        auto heap = HeapCreate(0, 0, 0);
        auto array = (int *) HeapAlloc(heap, 0, 1024 * 1024 * 1024);
        count--;
        in3.close();

        std::ifstream in(file);

        for (int i = 0; i < count; i++) {
            in >> temp;
            array[i] = temp;
        }

        auto startTime = static_cast<int>(clock());

        sortArray(array, count);
        findInArray(array, 4, count);

        auto endTime = static_cast<int>(clock());
        in.close();
        return endTime - startTime;
    }
}

int spo1(int argc, char **argv) {
    int buf, heapTime, fileTime, baseTime;

    const int FILES_COUNT = 5;

    char *files[] = {const_cast<char *>("2048.txt"),
                     const_cast<char *>("4096.txt"),
                     const_cast<char *>("8192.txt"),
                     const_cast<char *>("16384.txt"),
                     const_cast<char *>("32768.txt")};

    printHeading();
    for (int j = 0; j < FILES_COUNT; j++) {
        int count = 0;
        char *file = files[j];
        std::ifstream in(file);

        if (in.is_open()) {
            while (!in.eof()) {
                in >> buf;
                count++;
            }
            count--;
            in.close();
        }

        baseTime = basePointers(file, count);
        if (baseTime < 0) return baseTime;

        fileTime = mappingFiles(file, count);
        if (fileTime < 0) return fileTime;

        heapTime = heap1(file, count);
        if (heapTime < 0) return heapTime;

        printLine(count, heapTime, fileTime, baseTime);

        if (j < FILES_COUNT - 1) printSeparator(); else printFooter();
    }
    return 0;
}

//extern "C" __declspec(dllexport) int spo2() {
//    main(0, nullptr);
//}