#include <iostream>
#include <windows.h>

using namespace std;

int main(int argc, CHAR *argv[]) {
    int key;
    typedef void(*myFunction)();
    HMODULE libModule = LoadLibrary(TEXT("dll"));
    if (libModule == NULL) {
        cout << "dll error\n";
        FreeLibrary(libModule);
        return 0;
    }
    myFunction test;

    while (true) {
        cout << "1 - spo 1 " << endl;
        cout << "2 - spo 2 " << endl;
        cout << "3 - asm 1 " << endl;
        cout << "4 - asm 2 " << endl;
        cout << "5 - exit" << endl;
        cout << "Enter:" << endl;

        cin >> key;
        switch (key) {
            case 1: {
                test = (myFunction) GetProcAddress(libModule, "spo1");
                test();
                break;
            }
            case 2: {
                test = (myFunction) GetProcAddress(libModule, "spo2");
                test();
                break;
            }
            case 3: {
                test = (myFunction) GetProcAddress(libModule, "asm1");
                test();
                break;
            }
            case 4: {
                test = (myFunction) GetProcAddress(libModule, "asm2");
                test();
                break;
            }
            default: {
                FreeLibrary(libModule);
                return 0;
            }
        }
    }
    return 0;
}