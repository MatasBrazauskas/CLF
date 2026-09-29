#pragma once

#include <string_view>
#include <optional>

constexpr std::size_t operator ""_KB(unsigned long long t_val) noexcept {
    return t_val * 1024;
}

#include "StatHandler.hpp"

struct Inputs {
    Inputs();
    std::string_view fileName;
    int n;
    int reserveCount;
};

std::optional<Inputs> validateInputs(int argc, char **argv);
void printInfo(const Stats&);
