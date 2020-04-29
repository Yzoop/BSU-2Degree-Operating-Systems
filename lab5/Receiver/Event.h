//
// Created by Asus on 29.04.2020.
//

#ifndef PROCESS_SYNCHRONIZATION_CLION_SOLUTION_EVENT_H
#define PROCESS_SYNCHRONIZATION_CLION_SOLUTION_EVENT_H
#include <Windows.h>
#include <string>
#include "WinException.h"

class Event
{
public:
    Event(BOOL bManualReset, BOOL bInitialState, std::string lpName);
    Event(DWORD dwDesiredAccess, BOOL bInheritHandle, std::string lpName);
    HANDLE get_event_handle();
    void set_event();
    ~Event();

private:
    HANDLE hEvent;
};
#endif //PROCESS_SYNCHRONIZATION_CLION_SOLUTION_EVENT_H
