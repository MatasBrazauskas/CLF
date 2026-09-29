#include "FileHandler.hpp"

#include <string>
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "StatHandler.hpp"

FileHandler::FileHandler(const std::string_view filePath) {
    const int fd = open(filePath.data(), O_RDONLY);

    if (fd == -1) {
        throw std::runtime_error{"Cannot open file"};
    }

    struct stat sb{};
    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error("Error getting to end of file");
    }

    if (sb.st_size == 0) {
        close(fd);
        throw std::runtime_error{"Cannot retrieve stats about a file."};
    }

    char *memoryMappedFile = static_cast<char *>(mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0));
    close(fd);

    if (memoryMappedFile == MAP_FAILED) {
        throw std::runtime_error("Failed to create memory map");
    }

    if (madvise(memoryMappedFile, sb.st_size, MADV_SEQUENTIAL) == -1) {
        throw std::runtime_error("Failed to create memory map");
    }

    mappedFile = std::string_view{memoryMappedFile, static_cast<std::size_t>(sb.st_size)};
}

std::string_view FileHandler::getLine(const Simd& t_simd) {
    if (mappedFile.empty()) {
        return "";
    }

    while (index < mappedFile.size()) {
        const auto endLineIndex = t_simd.findNewLine(this->mappedFile, index);

        if (endLineIndex != std::string::npos) [[unlikely]] {
            const size_t len = endLineIndex - index;
            const std::size_t tempIndex{index};
            index = endLineIndex + 1;

            return mappedFile.substr(tempIndex, len);
        }
        return mappedFile.substr(index);
    }

    return "";
}

FileHandler::~FileHandler() {
    if (not mappedFile.empty()) {
        munmap(const_cast<char *>(mappedFile.data()), mappedFile.size());
    }
}
