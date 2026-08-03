#pragma once

#include <string>
#include <optional>

struct LineInfo {
    LineInfo() = delete;
    LineInfo(std::string_view t_ipAddress, std::string_view t_identProtocol, std::string_view t_userId, std::string_view t_date, std::string_view t_clientRequest, int t_statusCode, int t_byteCount);

    std::string_view ipAddress;
    std::string_view identProtocol;
    std::string_view userId;
    std::string_view date;
    std::string_view clientRequest;
    int statusCode;
    const int byteCount;
};

std::optional<LineInfo> parseLine(std::string_view line);