//
// Created by Asus on 29.04.2020.
//

#ifndef PROCESS_SYNCHRONIZATION_CLION_SOLUTION_MUTEX_H
#define PROCESS_SYNCHRONIZATION_CLION_SOLUTION_MUTEX_H

#include <Windows.h>
#include <string>
#include "WinException.h"

class Mutex
{
public:
    Mutex(std::string name);
    Mutex(DWORD dwDesiredAccess, BOOL bInitialOwner, std::string lpName);
    HANDLE get_handle_value();
    void acquire();
    void release();
    ~Mutex();
private:
    HANDLE hMutex;
};


#endif //PROCESS_SYNCHRONIZATION_CLION_SOLUTION_MUTEX_H