#include "LineParser.hpp"

#include <stdexcept>

LineInfo::LineInfo(std::string_view t_ipAddress, std::string_view t_identProtocol, std::string_view t_userId, std::string_view t_date, std::string_view t_clientRequest, const int t_statusCode, const int t_byteCount)
    : ipAddress{t_ipAddress}, identProtocol{t_identProtocol}, userId{t_userId}, date{t_date}, clientRequest{t_clientRequest}, statusCode{t_statusCode}, byteCount{t_byteCount} {}

static int parseStrViewToInt(const std::string_view str) {
    if (str == "-") [[unlikely]] {
        throw std::runtime_error("");
    }

    int val{};
    for (int i{}; i < str.length(); ++i) {
        val = val * 10 + (str[i] - '0');
    }

    return val;
}

std::optional<LineInfo> parseLine(Simd& t_simd, std::string_view line) {
    try {
        const std::size_t ipAddressStartIndex = 0;
        const std::size_t ipAddressEndIndex = t_simd.findDelimiter(line, ipAddressStartIndex, Delimiter::Spaces);
        const auto ipAddress = line.substr(ipAddressStartIndex, ipAddressEndIndex - ipAddressStartIndex);

        const std::size_t identProtocolStartIndex = ipAddressEndIndex + 1;
        const std::size_t identProtocolEndIndex = t_simd.findDelimiter(line, identProtocolStartIndex, Delimiter::Spaces);
        const auto identProtocol = line.substr(identProtocolStartIndex, identProtocolEndIndex - identProtocolStartIndex);

        const std::size_t userIdStartIndex = identProtocolEndIndex + 1;
        const std::size_t userIdEndIndex = t_simd.findDelimiter(line, userIdStartIndex, Delimiter::Spaces);
        const auto userId = line.substr(userIdStartIndex, userIdEndIndex - userIdStartIndex);

        const std::size_t dateStartIndex = userIdEndIndex + 2;
        const std::size_t dateEndIndex = t_simd.findDelimiter(line, dateStartIndex, Delimiter::Braces);
        const auto date = line.substr(dateStartIndex, dateEndIndex - dateStartIndex);

        const std::size_t clientRequestStartIndex = dateEndIndex + 3;
        const std::size_t clientRequestEndIndex = t_simd.findDelimiter(line, clientRequestStartIndex, Delimiter::Quotes);
        const auto clientRequest = line.substr(clientRequestStartIndex, clientRequestEndIndex - clientRequestStartIndex);

        const std::size_t statusCodeStartIndex = clientRequestEndIndex + 2;
        const std::size_t statusCodeEndIndex = t_simd.findDelimiter(line, statusCodeStartIndex, Delimiter::Spaces);
        const auto statusCodeStr = line.substr(statusCodeStartIndex, statusCodeEndIndex - statusCodeStartIndex);
        const int statusCode = parseStrViewToInt(statusCodeStr);

        const std::size_t byteCountStartIndex = statusCodeEndIndex + 1;
        const auto byteCountStr = line.substr(byteCountStartIndex);
        const int byteCount = parseStrViewToInt(byteCountStr);

        return LineInfo{ipAddress, identProtocol, userId, date, clientRequest, statusCode, byteCount};
    } catch (std::exception& _) {
        return std::nullopt;
    }
}
