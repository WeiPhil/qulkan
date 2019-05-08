#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <string>

#define ASSERT(boolean, message)                                                                                                                               \
    {                                                                                                                                                          \
        if (boolean != true) {                                                                                                                                 \
            std::cout << "[ASSERT FAILED] " << message << std::endl;                                                                                           \
            assert(false);                                                                                                                                     \
        }                                                                                                                                                      \
    }

#define C_STRING(value) std::to_string(value).c_str()

#endif