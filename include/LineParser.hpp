#pragma once

#include "Simd.hpp"

#include <string>
#include <optional>

struct LineInfo {
    LineInfo() = delete;
    LineInfo(std::string_view t_ipAddress, std::string_view t_identProtocol, std::string_view t_userId, std::string_view t_date, std::string_view t_clientRequest, int t_statusCode, int t_byteCount, int t_offset) noexcept;

    std::string_view ipAddress;
    std::string_view identProtocol;
    std::string_view userId;
    std::string_view date;
    std::string_view clientRequest;
    int statusCode;
    int byteCount;
    int offset;
};

std::optional<LineInfo> parseLine(Simd&, std::string_view) noexcept;