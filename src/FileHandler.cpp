#include "FileHandler.hpp"

#include <string>
#include <print>
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "StatHandler.hpp"

static const std::size_t page_size = static_cast<std::size_t>(sysconf(_SC_PAGESIZE));
const std::size_t page_mask = ~(page_size - 1);

FileHandler::FileHandler(Simd& t_simd, const std::string_view filePath, const std::size_t t_chunks) {
    fileChunks.reserve(t_chunks);

    const auto fd = open(filePath.data(), O_RDONLY);

    if (fd == -1) {
        throw std::runtime_error{"Cannot open file"};
    }

    struct stat sb{};
    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error("Error getting to end of file");
    }

    mappedFileSize = sb.st_size;
    if (mappedFileSize == 0) {
        close(fd);
        return;
    }

    mappedFile = static_cast<char *>(mmap(nullptr, mappedFileSize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0));
    close(fd);

    if (mappedFile == MAP_FAILED) {
        throw std::runtime_error("Failed to create memory map");
    }


    const auto chunkSize = mappedFileSize / t_chunks;
    std::size_t startOfChunkIndex = 0;
    std::size_t totalBytesProcessed = 0;

    for (std::size_t i = 0; i + 1 < t_chunks && startOfChunkIndex < mappedFileSize; ++i) {
        const auto endOfChunkIndex = startOfChunkIndex + chunkSize;

        if (endOfChunkIndex >= mappedFileSize) {
            break;
        }

        // Clamp search window size to prevent reading past mappedFileSize
        const std::size_t maxSearchLen = std::min<std::size_t>(200, mappedFileSize - endOfChunkIndex);
        const auto searchWindow = std::string_view{mappedFile + endOfChunkIndex, maxSearchLen};

        const auto index = t_simd.findNewLine(searchWindow);

        // Calculate chunk length including the '\n' byte
        const std::size_t currentChunkLen = (endOfChunkIndex + index + 1) - startOfChunkIndex;

        const auto temp = std::string_view{mappedFile + startOfChunkIndex, currentChunkLen};
        fileChunks.emplace_back(temp);

        totalBytesProcessed += currentChunkLen;
        startOfChunkIndex = endOfChunkIndex + index + 1; // Move past the '\n'
    }

    // Add remaining tail of the file as the final chunk
    if (startOfChunkIndex < mappedFileSize) {
        const std::size_t tailLen = mappedFileSize - startOfChunkIndex;
        fileChunks.emplace_back(mappedFile + startOfChunkIndex, tailLen);
        totalBytesProcessed += tailLen;
    }

    std::println("Sum {} and file size {}", totalBytesProcessed, mappedFileSize);

    /*const auto chunkSize{mappedFileSize / t_chunks};
    std::size_t startOfChunkIndex{};

    auto sum{0};

    for (std::size_t i{}; i + 1 < t_chunks and startOfChunkIndex < mappedFileSize; ++i) {
        const auto endOfChunkIndex = startOfChunkIndex + chunkSize;

        if (endOfChunkIndex >= mappedFileSize) {
            throw std::runtime_error{"BLET"};
        }

        const auto chunk = std::string_view{mappedFile + endOfChunkIndex, 200};
        const auto index = t_simd.findNewLine(chunk);

        const auto temp = std::string_view{mappedFile + startOfChunkIndex, index + chunkSize};
        fileChunks.emplace_back(temp);
        std::println("{}", index + chunkSize);
        sum += index + chunkSize;
        startOfChunkIndex = endOfChunkIndex + index + 1;
    }

    std::println("Sum {} and file size {}", sum, mappedFileSize);*/

    fileChunks.emplace_back(mappedFile + startOfChunkIndex, mappedFileSize - startOfChunkIndex);
    /*while (fileChunks.size() < t_chunks) {
        fileChunks.emplace_back();
    }

    for (auto& chunk : fileChunks) {
        if (chunk.empty()) {
            continue;
        }

        const auto addr = reinterpret_cast<std::uintptr_t>(chunk.data());

        const auto aligned_addr = addr & page_mask;
        const std::size_t delta = addr - aligned_addr;

        const std::size_t aligned_len = (chunk.size() + delta + page_size - 1) & page_mask;

        if(madvise(reinterpret_cast<void*>(aligned_addr), aligned_len, MADV_SEQUENTIAL) == -1) {
            throw std::runtime_error("Failed to create memory map");
        }
    }*/
}

std::generator<std::string_view> FileHandler::getLine(const std::size_t t_index) const {
    if (fileChunks[t_index].empty()) {
        co_return;
    }

    std::size_t newLineIndex{};
    while (newLineIndex < fileChunks[t_index].size()) {
        const std::size_t endLineIndex = fileChunks[t_index].find_first_of('\n', newLineIndex);

        if (endLineIndex != std::string::npos) {
            const size_t len = endLineIndex - newLineIndex;

            co_yield fileChunks[t_index].substr(newLineIndex, len);

            newLineIndex = endLineIndex + 1;
        } else {
            co_yield fileChunks[t_index].substr(newLineIndex);
            break;
        }
    }
}

[[nodiscard]] const char* FileHandler::getChunkStartAddress(const std::size_t) const {
    return fileChunks.front().data();
}

FileHandler::~FileHandler() {
    if (mappedFile != nullptr) {
        munmap(mappedFile, mappedFileSize);
    }
}
