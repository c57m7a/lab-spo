#include <windows.h>
#include <iostream>
#include <locale.h>
#include <fstream>
#include <cstdlib>
#include <time.h>

void sortArray(int *array, int size) {
    // Сортировка методом выбора

    for (int i = 0; i < size - 1; i++) {
        int min = array[i];
        for (int y = i + 1; y < size; y++) {
            if (min > array[y]) {
                min = array[y];
                array[y] = array[i];
                array[i] = min;
            }
        }
    }
}

void findInArray(int *array, int key, int size) {
    // Бинарный поиск
    int left, right, search = -1;
    left = 0;
    right = size - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        if (key == array[mid]) {
            search = mid;
            break;
        }
        if (key < array[mid])
            right = mid - 1;
        else
            left = mid + 1;
    }
    if (search == -1)
        std::cout << "Not found\n";
    else
        std::cout << "Found \"" << array[search] << "\"\n";
}


int main(int argc, char *argv[]) {
    int count, startTime, endTime;

    startTime = static_cast<int>(clock());

    std::ifstream in(argv[2]);

    if (in.is_open()) {
        count = 0;
        int temp;
        while (!in.eof()) {
            in >> temp;
            count++;
        }
        count--;
        int *array = new int[count];
        in.close();
        std::ifstream in1(argv[2]);
        for (int i = 0; i < count; i++) {
            in1 >> temp;
            array[i] = temp;
        }
        in1.close();

        sortArray(array, count);
        temp = atoi(argv[1]);
        findInArray(array, temp, count);
        std::cout << "Process id - " << GetCurrentProcessId() << "\n";
    }
    endTime = static_cast<int>(clock());
    std::cout << "Time: " << endTime - startTime << "ms. \n";
    Sleep(1000);
    return 0;
}