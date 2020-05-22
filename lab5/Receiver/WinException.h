//
// Created by Asus on 29.04.2020.
//

#ifndef PROCESS_SYNCHRONIZATION_CLION_SOLUTION_WINEXCEPTION_H
#define PROCESS_SYNCHRONIZATION_CLION_SOLUTION_WINEXCEPTION_H
#include <iostream>

class WinException :public std::exception
{
public:
    WinException(const char* msg, int error_code);

    WinException(exception msg, int error_code);

    int get_error_code();
private:
    int error_code;
};
#endif //PROCESS_SYNCHRONIZATION_CLION_SOLUTION_WINEXCEPTION_H
