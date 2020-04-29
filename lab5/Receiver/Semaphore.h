//
// Created by Asus on 29.04.2020.
//

#ifndef PROCESS_SYNCHRONIZATION_CLION_SOLUTION_SEMAPHORE_H
#define PROCESS_SYNCHRONIZATION_CLION_SOLUTION_SEMAPHORE_H

#include <Windows.h>
#include <string>
#include "WinException.h"

class Semaphore
{
public:
    Semaphore(LONG lInitialCount, LONG lMaximumCount, std::string name);
    Semaphore(DWORD dwDesiredAccess, BOOL bInheritHandle, std::string lpName);
    HANDLE get_handle_value();
    void signal();
    void wait();
    ~Semaphore();

private:
    HANDLE hSemaphore;
};
#endif //PROCESS_SYNCHRONIZATION_CLION_SOLUTION_SEMAPHORE_H
