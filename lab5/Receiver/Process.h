//
// Created by Asus on 29.04.2020.
//

#ifndef PROCESS_SYNCHRONIZATION_CLION_SOLUTION_PROCESS_H
#define PROCESS_SYNCHRONIZATION_CLION_SOLUTION_PROCESS_H
#include <Windows.h>
#include <string>
#include "WinException.h"
class Process
{
public:
    explicit Process(std::string lpCommandLineCreator);
    DWORD work();
    HANDLE get_handle_value();
    ~Process();

private:
    HANDLE hProcess;
    STARTUPINFO siCreator;
    PROCESS_INFORMATION piCreator;
    std::string lpCommandLine;
};
#endif //PROCESS_SYNCHRONIZATION_CLION_SOLUTION_PROCESS_H
