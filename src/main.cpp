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

void mergeStats(StatHandler& t_og, StatHandler& t_other, const char* t_chunkStart) {
    t_og.merge(t_other, t_chunkStart);
}

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

    {
        std::vector<std::jthread> firstLine;
        firstLine.reserve(8);

        for (auto i{0}; i < 8; i++) {
            const auto currIndex = i * 2;
            const auto nextIndex = i * 2 + 1;
            const auto chuckStart = fileHandler.getChunkStartAddress(nextIndex);
            firstLine.emplace_back(mergeStats, std::ref(statHandlers[currIndex]), std::ref(statHandlers[nextIndex]), chuckStart);
        }
    }

    {
        std::vector<std::jthread> secondLine;
        secondLine.reserve(4);

        for (auto i{0}; i < 4; i++) {
            const auto currIndex = i * 4;
            const auto nextIndex = i * 4 + 2;
            const auto chuckStart = fileHandler.getChunkStartAddress(nextIndex);
            secondLine.emplace_back(mergeStats, std::ref(statHandlers[currIndex]), std::ref(statHandlers[nextIndex]), chuckStart);
        }
    }

    {
        std::vector<std::jthread> thirdLine;
        thirdLine.reserve(2);

        for (auto i{0}; i < 2; i++) {
            const auto currIndex = i * 8;
            const auto nextIndex = i * 8 + 4;
            const auto chuckStart = fileHandler.getChunkStartAddress(nextIndex);
            thirdLine.emplace_back(mergeStats, std::ref(statHandlers[currIndex]), std::ref(statHandlers[nextIndex]), chuckStart);
        }
    }

    const auto temp = fileHandler.getChunkStartAddress(8);
    statHandlers[0].merge(statHandlers[8], temp);

    const auto stats = statHandlers[0].retrieveStats(inputs.n, fileHandler.getChunkStartAddress(0));
    printInfo(stats);
}