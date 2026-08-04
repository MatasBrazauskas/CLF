#include "FileHandler.hpp"

#include <string>
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "StatHandler.hpp"

FileHandler::FileHandler(const std::string& filePath) {
    const int fd = open(filePath.c_str(), O_RDONLY);

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
        return;
    }

    char *memoryMappedFile = static_cast<char *>(mmap(nullptr, sb.st_size, PROT_READ, MAP_SHARED, fd, 0));
    close(fd);

    if (memoryMappedFile == MAP_FAILED) {
        throw std::runtime_error("Failed to create memory map");
    }

    madvise(memoryMappedFile, sb.st_size, MADV_SEQUENTIAL);

    mappedFile = std::string_view{memoryMappedFile, static_cast<std::size_t>(sb.st_size)};
}

std::generator<std::string_view> FileHandler::getLine() const {
    if (mappedFile.empty()) {
        co_return;
    }

    std::size_t newLineIndex{};
    while (newLineIndex < mappedFile.size()) {
        const std::size_t endLineIndex = this->mappedFile.find_first_of('\n', newLineIndex);

        if (endLineIndex != std::string::npos) {
            const size_t len = endLineIndex - newLineIndex;

            co_yield mappedFile.substr(newLineIndex, len);

            newLineIndex = endLineIndex + 1;
        } else {
            co_yield mappedFile.substr(newLineIndex);
            break;
        }
    }
}

FileHandler::~FileHandler() {
    if (mappedFile.empty() == false) {
        munmap(const_cast<char *>(mappedFile.data()), mappedFile.size());
    }
}
