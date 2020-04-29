//
// Created by Asus on 29.04.2020.
//
#include "WinException.h"


WinException::WinException(exception msg, int error_code): exception(msg)
{
    this->error_code = error_code;
}

int WinException::get_error_code()
{
    return this->error_code;
}

