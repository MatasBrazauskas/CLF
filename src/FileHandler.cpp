#include "FileHandler.hpp"

#include <string>
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "StatHandler.hpp"

static const std::size_t page_size = static_cast<std::size_t>(sysconf(_SC_PAGESIZE));
const std::size_t page_mask = ~(page_size - 1);

FileHandler::FileHandler(const std::string_view filePath, const std::size_t t_chunks, Simd& t_simd) {
    fileChunks.reserve(t_chunks);

    const int fd = open(filePath.data(), O_RDONLY);

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


    const uint chunkSize = mappedFileSize / (t_chunks);
    uint startOfChunk{};
    for (int i{}; i < t_chunks - 1; ++i) {
        const auto endOfChunk = startOfChunk + chunkSize;
        const auto chunk = std::string_view{mappedFile + endOfChunk, chunkSize};
        const std::size_t index = t_simd.findNewLine(chunk);

        fileChunks.emplace_back(mappedFile + startOfChunk, index + endOfChunk - startOfChunk);
        startOfChunk = endOfChunk + index + 1;
    }

    fileChunks.emplace_back(mappedFile + startOfChunk, mappedFileSize - startOfChunk);

    for (auto& chunk : fileChunks) {
        const auto addr = reinterpret_cast<std::uintptr_t>(chunk.data());

        const auto aligned_addr = addr & page_mask;
        const std::size_t delta = addr - aligned_addr;

        const std::size_t aligned_len = (chunk.size() + delta + page_size - 1) & page_mask;

        if(madvise(reinterpret_cast<void*>(aligned_addr), aligned_len, MADV_SEQUENTIAL) == -1) {
            throw std::runtime_error("Failed to create memory map");
        }
    }
}

std::generator<std::string_view> FileHandler::getLine(const int t_index) const {
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

FileHandler::~FileHandler() {
    if (mappedFile == nullptr) {
        munmap(mappedFile, mappedFileSize);
    }
}
