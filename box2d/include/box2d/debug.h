//
// Created by Daniel on 30.07.2024.
//

#ifndef BOX2D_DEBUG_H
#define BOX2D_DEBUG_H

#endif //BOX2D_DEBUG_H
#pragma once

#include <emscripten/emscripten.h>
#include <string>
#include <sstream>
#include <iomanip>

inline void print(const std::string &str) {
    EM_ASM({
        console.log(UTF8ToString($0));
        }, str.c_str());
};
inline std::string floatToString(float value) {
    std::ostringstream out;
    out << std::setprecision(std::numeric_limits<float>::max_digits10) << value;
    return out.str();
}