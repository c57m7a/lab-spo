#include <string>
#include <cstdio>
#include <cerrno>
#include <clocale>
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#define BUF_SIZE 256
#define ARRAY_SIZE 128
#define N 10000
#define PRINT FALSE

typedef struct {
    char *surname;
    int semester;
    char *discipline;
    int mark;
} ResultsInfo;

int copyC(char *in_file_path, char *out_file_path) {
    FILE *in_file, *out_file;
    char buf[BUF_SIZE];
    size_t bytes_in, bytes_out;

    in_file = fopen(in_file_path, "rb");
    if (in_file == nullptr) {
        perror(in_file_path);
        return 2;
    }

    out_file = fopen(out_file_path, "wb");
    if (out_file == nullptr) {
        perror(out_file_path);
        return 3;
    }

    while ((bytes_in = fread(buf, 1, BUF_SIZE, in_file)) > 0) {
        bytes_out = fwrite(buf, 1, bytes_in, out_file);
        if (bytes_out != bytes_in) {
            perror("Ошибка записи");
            return 4;
        }
    }

    fclose(in_file);
    fclose(out_file);
    return 0;
}

int copyWindows(LPTSTR in_file_path, LPTSTR out_file_path) {
    HANDLE hIn, hOut;
    DWORD nIn, nOut;
    CHAR Buffer[BUF_SIZE];
    hIn = CreateFile(in_file_path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hIn == INVALID_HANDLE_VALUE) {
        printf("Невозможно открыть входной файл. Ошибка: %x\n", GetLastError());
        return 2;
    }
    hOut = CreateFile(out_file_path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hOut == INVALID_HANDLE_VALUE) {
        printf("Невозможно открыть выходной файл. Ошибка: %x\n", GetLastError());
        return 3;
    }
    while (ReadFile(hIn, Buffer, BUF_SIZE, &nIn, nullptr) && nIn > 0) {
        WriteFile(hOut, Buffer, nIn, &nOut, nullptr);
        if (nIn != nOut) {
            printf("Ошибка записи: %x\n", GetLastError());
            return 4;
        }
    }
    CloseHandle(hIn);
    CloseHandle(hOut);
    return 0;
}

int copyWindowsCopyFile(LPTSTR in_file_path, LPTSTR out_file_path) {
    if (!CopyFile(in_file_path, out_file_path, FALSE)) {
        printf("Ошибка при выполнении функции CopyFile: %x\n", GetLastError());
        return 2;
    }
    return 0;
}

void generateFile(char *file_path) {
    char *disciplines[] = {const_cast<char *>("Culturology     "),
                           const_cast<char *>("Mathematics     "),
                           const_cast<char *>("Ecology         "),
                           const_cast<char *>("History         "),
                           const_cast<char *>("Computer science")};

#if PRINT
    printf("Семестр |Фамилия\t|Дисциплина\t|Баллы\n");
    printf("-----------------------------------------------\n");
#endif

    FILE *fp = fopen(file_path, "w");
    srand(static_cast<unsigned int>(time(nullptr)));
    double averageRating = 0;

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        auto result = (ResultsInfo *) malloc(sizeof(ResultsInfo));
        result->semester = rand() % 3 + 1;
        result->surname = const_cast<char *>("Kuznetsov");
        result->discipline = disciplines[rand() % (sizeof(disciplines) / sizeof(disciplines[0]))];
        result->mark = rand() % 101;
        averageRating += result->mark;

        fprintf(fp, "%d %s %s %d\n", result->semester, result->surname, result->discipline, result->mark);
#if PRINT
        printf("%d|%s|%s|%d\n", result->semester, result->surname, result->discipline, result->mark);
#endif
    }
    averageRating /= ARRAY_SIZE;

    fclose(fp);
    printf("Средняя оценка: %.2f\n", averageRating);
}

//char args[3][] = {"", "in.txt", "out.txt"};
//extern "C" __declspec(dllexport) int spo1() {
//    main(3, args)
//}

int main(int argc, char *argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc != 4 && argc != 5) {
        printf("Аргументы: файл1.txt файл2.txt key\n");
        return 1;
    }

    char *in_file = argv[1];
    char *out_file = argv[2];
    std::string key(argv[3]);

    if (argc > 4) {
        std::string option(argv[4]);
        if (option == "-g") {
            generateFile(in_file);
        }
    }

    int startMsAll = GetTickCount();

    if (key == "-c") {
        copyC(in_file, out_file);
    } else if (key == "-w") {
        copyWindows(in_file, out_file);
    } else if (key == "-wcf") {
        copyWindowsCopyFile(in_file, out_file);
    } else if (key == "-all") {
        int startMs = GetTickCount();
        for (int i = 0; i < N; ++i) copyC(in_file, out_file);
        int endMs = GetTickCount();
        std::cout << "C: " << endMs - startMs << " мс\n";

        startMs = GetTickCount();
        for (int i = 0; i < N; ++i) copyWindows(in_file, out_file);
        endMs = GetTickCount();
        std::cout << "Windows: " << endMs - startMs << " мс\n";

        startMs = GetTickCount();
        for (int i = 0; i < N; ++i) copyWindowsCopyFile(in_file, out_file);
        endMs = GetTickCount();
        std::cout << "Windows CopyFile: " << endMs - startMs << " мс\n";
    } else {
        printf("Способ не выбран");
        return 2;
    }

    int endMsAll = GetTickCount();
    std::cout << "Общее время: " << endMsAll - startMsAll << " мс\n";
    return 0;
}