#include <string>
#include <iostream>
#include <stdint.h>

extern "C" __declspec(dllexport)
const char * hello(const char *name)
{
    std::cout << "From CPP: " << name << std::endl;
    static thread_local std::string result;
    result = std::string("Hello ") + name;
    return result.c_str();
}
