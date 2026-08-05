#include "FileHandler.hpp"

#include <string>

FileHandler::FileHandler(std::string_view filePath): fileStream{std::ifstream{filePath.data()}} {}

std::generator<std::string> FileHandler::getLine() {

    std::string line;
    while (std::getline(fileStream, line)) {
        co_yield line;
    }
}

FileHandler::~FileHandler() {
    fileStream.close();
}
