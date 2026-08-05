#pragma once

#include <string_view>
#include <optional>

struct Inputs {
    Inputs();
    std::string_view fileName;
    int n;
};

std::optional<Inputs> validateInputs(int argc, char **argv);
