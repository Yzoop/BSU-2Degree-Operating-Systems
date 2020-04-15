// OS_createProcess.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>

using namespace std;

int main()
{
    STARTUPINFO cif;

    ZeroMemory(&cif, sizeof(STARTUPINFO));
    PROCESS_INFORMATION pi;


    cif.cb = sizeof(STARTUPINFO);


    if (CreateProcess(L"D:/BSU/2Degree/4SEM/OS/LABS/OS_createProcess/Debug/parallelProgram.exe", NULL,
        NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &cif, &pi))
    {
        cout << "process" <<  endl;
        cout << "handle " << pi.hProcess << endl;

    }
    else
    {
        cout << "Error faced: " << GetLastError();
    }
}
