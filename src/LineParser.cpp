#include "LineParser.hpp"

LineInfo::LineInfo(const std::string_view t_ipAddress, const std::string_view t_identProtocol, const std::string_view t_userId, const std::string_view t_date, const std::string_view t_clientRequest, const int t_statusCode, const int t_byteCount, const int t_offset) noexcept
    : ipAddress{t_ipAddress}, identProtocol{t_identProtocol}, userId{t_userId}, date{t_date}, clientRequest{t_clientRequest}, statusCode{t_statusCode}, byteCount{t_byteCount}, offset{t_offset} {}

static int parseStrViewToInt(const std::string_view str) noexcept {
    int val{};
    for (int i{}; i < str.length(); ++i) {
        val = val * 10 + (str[i] - '0');
    }

    return val;
}

std::optional<LineInfo> parseLine(Simd& t_simd, const std::string_view line) noexcept {
    const std::size_t ipAddressEndIndex = t_simd.findDelimiter(line, 0, Delimiter::Spaces);
    const auto ipAddress = line.substr(0, ipAddressEndIndex);

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
    const auto offset = t_simd.findNewLine(line, byteCountStartIndex);
    const auto byteCountStr = line.substr(byteCountStartIndex, offset - byteCountStartIndex);
    const int byteCount = parseStrViewToInt(byteCountStr);

    return LineInfo{ipAddress, identProtocol, userId, date, clientRequest, statusCode, byteCount, static_cast<int>(offset + 1)};
}
