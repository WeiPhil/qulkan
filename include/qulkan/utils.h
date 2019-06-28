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

namespace Qulkan {
    namespace {
        static int uniqueID = 0;
        static uint64_t frameNumber = 0;
    } // namespace

    static int getNextUniqueID() { return ++uniqueID; }
    static void incrementFrameNumber() { ++frameNumber; }
    static uint64_t getFrameNumber() { return frameNumber; }
} // namespace Qulkan
#endif