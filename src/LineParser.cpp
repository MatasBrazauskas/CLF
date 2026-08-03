#include "LineParser.hpp"

LineInfo::LineInfo(const std::string& t_ipAddress, const std::string& t_identProtocol, const std::string& t_userId, const std::string& t_date, const std::string& t_clientRequest, const int t_statusCode, const int t_byteCount)
    : ipAddress{t_ipAddress}, identProtocol{t_identProtocol}, userId{t_userId}, date{t_date}, clientRequest{t_clientRequest}, statusCode{t_statusCode}, byteCount{t_byteCount} {}

std::optional<LineInfo> parseLine(const std::string& line) {
    try {
        const std::size_t ipAddressStartIndex = 0;
        const std::size_t ipAddressEndIndex = line.find(' ', ipAddressStartIndex);
        const std::string ipAddress = line.substr(ipAddressStartIndex, ipAddressEndIndex - ipAddressStartIndex);

        const std::size_t identProtocolStartIndex = ipAddressEndIndex + 1;
        const std::size_t identProtocolEndIndex = line.find(' ', identProtocolStartIndex);
        const std::string identProtocol = line.substr(identProtocolStartIndex, identProtocolEndIndex - identProtocolStartIndex);

        const std::size_t userIdStartIndex = identProtocolEndIndex + 1;
        const std::size_t userIdEndIndex = line.find(' ', userIdStartIndex);
        const std::string userId = line.substr(userIdStartIndex, userIdEndIndex - userIdStartIndex);

        const std::size_t dateStartIndex = userIdEndIndex + 2;
        const std::size_t dateEndIndex = line.find(']', dateStartIndex);
        const std::string date = line.substr(dateStartIndex, dateEndIndex - dateStartIndex);

        const std::size_t clientRequestStartIndex = dateEndIndex + 3;
        const std::size_t clientRequestEndIndex = line.find('"', clientRequestStartIndex);
        const std::string clientRequest = line.substr(clientRequestStartIndex, clientRequestEndIndex - clientRequestStartIndex);

        const std::size_t statusCodeStartIndex = clientRequestEndIndex + 2;
        const std::size_t statusCodeEndIndex = line.find(' ', statusCodeStartIndex);
        const std::string statusCodeStr = line.substr(statusCodeStartIndex, statusCodeEndIndex - statusCodeStartIndex);
        const int statusCode = std::stoi(statusCodeStr);

        const std::size_t byteCountStartIndex = statusCodeEndIndex + 1;
        const std::string byteCountStr = line.substr(byteCountStartIndex);
        const int byteCount = std::stoi(byteCountStr);

        return LineInfo{ipAddress, identProtocol, userId, date, clientRequest, statusCode, byteCount};
    } catch (std::exception& _) {
        return std::nullopt;
    }
}