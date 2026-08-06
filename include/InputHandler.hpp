#pragma once

#include <string_view>
#include <optional>

struct Inputs {
    Inputs();
    std::string_view fileName;
    int n;
    int reserveCount;
    int maps;
};

std::optional<Inputs> validateInputs(int argc, char **argv);