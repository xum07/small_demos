#include <iostream>
#include <stdarg.h>
#include "utils.h"

void cli_print(const char *errLevel, const char *file, size_t line, const char *format, ...) 
{
    std::cout << "[" << errLevel << "](" << file << ":" << line << ")  ";

    va_list va;
    va_start(va, format);
    vprintf(format, va);
    va_end (va);

    std::cout << std::endl;
}