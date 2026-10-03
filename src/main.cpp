#include <print>
#include <string>

#include "FileHandler.hpp"
#include "LineParser.hpp"
#include "InputHandler.hpp"
#include "StatHandler.hpp"

int main(const int argc, char** argv) {
    const auto inputsOpt = validateInputs(argc, argv);
    if (not inputsOpt.has_value()) [[unlikely]] {
        return -1;
    }

    const auto inputs = inputsOpt.value();

    StatHandler statHandler{static_cast<std::size_t>(inputs.reserveCount)};
    FileHandler fileHandler{inputs.fileName};
    Simd simd;

    std::string_view line;
    while ((line = fileHandler.getLine()) != emptyString) {
        const auto lineOpt = parseLine(simd, line);
        if (lineOpt.has_value()) [[likely]] {
            const auto lineInfo = lineOpt.value();
            statHandler.analyzeLine(lineInfo, fileHandler.mappedFile.data());
            fileHandler.addOffset(lineInfo.offset);
        }
    }

    const auto stats = statHandler.retrieveStats(inputs.n, fileHandler.mappedFile.data());
    printInfo(stats);
}