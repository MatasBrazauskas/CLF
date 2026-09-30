#include "InputHandler.hpp"
#include "StatHandler.hpp"
#include "FileHandler.hpp"
#include "LineParser.hpp"

#include <string>
#include <thread>
#include <print>

using namespace std::string_literals;

constexpr auto CThreadCount{16zu};
Simd fileSimd{};

void workerFunction(StatHandler& t_statHandler, const FileHandler& t_fileHandler, Simd& t_simd, const std::size_t t_index) {
    const auto chunkStartAddress = t_fileHandler.getChunkStartAddress(t_index);
    for (const std::string_view line : t_fileHandler.getLine(t_index)) {
        const auto lineInfo = parseLine(t_simd, line);

        if (lineInfo.has_value()) {
            const auto lineValue = lineInfo.value();
            t_statHandler.analyzeLine(lineValue, chunkStartAddress);
        }
    }
};

int main(const int argc, char** argv) {
    const auto inputsOpt = validateInputs(argc, argv);
    if (not inputsOpt.has_value()) {
        return -1;
    }

    const auto inputs = inputsOpt.value();

    std::array<Simd, CThreadCount> simd{};

    std::vector<StatHandler> statHandlers;
    statHandlers.reserve(CThreadCount);
    statHandlers.emplace_back(inputs.reserveCount * CThreadCount, true);
    for (auto i{1}; i < CThreadCount; ++i) {
        statHandlers.emplace_back(static_cast<std::size_t>(inputs.reserveCount));
    }

    FileHandler fileHandler{fileSimd, inputs.fileName, CThreadCount};

    std::vector<std::thread> workerThreads;
    workerThreads.reserve(CThreadCount);
    for (auto i{0}; i < CThreadCount; ++i) {
        workerThreads.emplace_back(workerFunction, std::ref(statHandlers[i]), std::ref(fileHandler) , std::ref(simd[i]), i);
    }

    for (auto& thread : workerThreads) {
        thread.join();
    }

    for (auto i{1}; i < CThreadCount; ++i) {
        statHandlers[0].merge(statHandlers[i], fileHandler.getChunkStartAddress(i));
    }

    const auto stats = statHandlers[0].retrieveStats(inputs.n, fileHandler.getChunkStartAddress(0));
    printInfo(stats);
}