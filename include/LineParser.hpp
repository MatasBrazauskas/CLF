#pragma once

#include <string>
#include <optional>

struct LineInfo {
    LineInfo() = delete;
    LineInfo(const std::string& t_ipAddress, const std::string& t_identProtocol, const std::string& t_userId, const std::string& t_date, const std::string& t_clientRequest, int t_statusCode, int t_byteCount);

    std::string ipAddress;
    std::string identProtocol;
    std::string userId;
    std::string date;
    std::string clientRequest;
    int statusCode;
    const int byteCount;
};

std::optional<LineInfo> parseLine(const std::string& line);