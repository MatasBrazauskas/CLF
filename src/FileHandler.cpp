#include "FileHandler.hpp"

#include <string>
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "StatHandler.hpp"

FileHandler::FileHandler(const std::string_view filePath, const uint t_chunks) {
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

    mappedFile = static_cast<char *>(mmap(nullptr, mappedFileSize, PROT_READ, MAP_SHARED, fd, 0));
    close(fd);

    if (mappedFile == MAP_FAILED) {
        throw std::runtime_error("Failed to create memory map");
    }


    const uint chunkSize = mappedFileSize / (t_chunks);
    uint startOfChunk{};
    for (int i{}; i < t_chunks - 1; ++i) {
        std::size_t index{};

        for (uint j{startOfChunk + chunkSize}; j < mappedFileSize; ++j) {
            if (mappedFile[j] == '\n') {
                index = j;
                break;
            }
        }

        if (index == 0) {
            throw std::runtime_error{"File chunk at index " + std::to_string(startOfChunk)};
        }

        fileChunks.emplace_back(mappedFile + startOfChunk, index - startOfChunk);
        startOfChunk = index + 1;
    }

    fileChunks.emplace_back(mappedFile + startOfChunk, mappedFileSize - startOfChunk);
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
