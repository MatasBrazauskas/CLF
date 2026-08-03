#include "FileHandler.hpp"

#include <string>

FileHandler::FileHandler(const std::string& filePath): fileStream{std::ifstream{filePath}} {}

std::generator<std::string> FileHandler::getLine() {

    std::string line;
    while (std::getline(fileStream, line)) {
        co_yield line;
    }
}

FileHandler::~FileHandler() {
    fileStream.close();
}
