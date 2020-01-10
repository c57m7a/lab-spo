#include <windows.h>
#include <cstdio>
#include <iostream>
#include <w32api/winuser.h>

using namespace std;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK keyboardHook;
volatile bool blocking = false;

HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, "lab6");

int main() {
    if (!hMutex) {
        CreateMutex(nullptr, 0, "lab6");
    } else {
        printf("The application is already running");
        return -1;
    }
    cout << "CTRL + SHIFT + O to block keyboard" << endl;
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
    MSG msg{nullptr};
    while (GetMessage(&msg, nullptr, 0, 0) != 0);
    UnhookWindowsHookEx(keyboardHook);
    CloseHandle(hMutex);
    return 0;
}


LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    auto key = (PKBDLLHOOKSTRUCT) lParam;

    if (wParam == WM_KEYDOWN && nCode == HC_ACTION) {
        if (key->vkCode != VK_LSHIFT) {
            if (key->vkCode == 'O') {
                blocking = !blocking;
                if (blocking)
                    cout << endl << "keyboard blocked" << endl;
                else
                    cout << endl << "keyboard unblocked" << endl;
            } else if (key->vkCode == 'Q') {
                UnhookWindowsHookEx(keyboardHook);
                CloseHandle(hMutex);
                ExitProcess(0);
            }
        }
    }

    if (!blocking) {
        return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
    } else {
        return 1;
    }
}