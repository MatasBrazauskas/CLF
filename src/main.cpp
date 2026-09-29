#include <print>

#include "FileHandler.hpp"
#include "LineParser.hpp"

#include <string>

#include "InputHandler.hpp"
#include "StatHandler.hpp"

using namespace std::string_literals;

int main(const int argc, char** argv) {
    const auto inputsOpt = validateInputs(argc, argv);
    if (not inputsOpt.has_value()) {
        return -1;
    }

    const auto inputs = inputsOpt.value();

    StatHandler statHandler{static_cast<std::size_t>(inputs.reserveCount)};
    FileHandler fileHandler{inputs.fileName};
    Simd simd;

    std::string_view line;
    while ((line = fileHandler.getLine(simd)) != "") {
        const auto lineInfo = parseLine(simd, line);
        if (lineInfo.has_value()) {
            statHandler.analyzeLine(lineInfo.value(), fileHandler.mappedFile.data());
        }
    }

    const auto stats = statHandler.retrieveStats(inputs.n, fileHandler.mappedFile.data());
    printInfo(stats);
}