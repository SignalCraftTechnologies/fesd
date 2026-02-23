#include "MessageBuilder.hpp"
#include <fesd/types/SC2470.hpp>

#include <map>

namespace {

// Special Characters
const std::string queryCharacter = "?";
const std::string paramDelim = " ";

const std::map<fesd::SC2470::Path, std::string> PathStringMap = {
    {fesd::SC2470::Path::RX,    "RX"},
    {fesd::SC2470::Path::TX,    "TX"},
};

} // static namespace

namespace fesd {

std::string MessageBuilder::buildCommand(const std::string& command, uint16_t channelId, const std::vector<std::string>& additionalParams)
{
    std::string message = command + paramDelim + std::to_string(channelId) + paramDelim;
    for(const std::string& param : additionalParams)
        message += param + paramDelim;
    return message;
}

std::string MessageBuilder::buildCommand(const std::string& command, uint16_t channelId, SC2470::Path path, const std::vector<std::string>& additionalParams)
{
    std::vector<std::string> copyAdditionalParams = additionalParams;
    copyAdditionalParams.insert(copyAdditionalParams.begin(), PathStringMap.at(path));
    return buildCommand(command, channelId, copyAdditionalParams);
}

std::string MessageBuilder::buildCommand(const std::string& command, uint16_t channelId, SC2470::Path path)
{
    return buildCommand(command, channelId, path, std::vector<std::string>());
}

std::string MessageBuilder::buildCommand(const std::string& command, uint16_t channelId)
{
    return buildCommand(command, channelId, std::vector<std::string>());
}

std::string MessageBuilder::buildQuery(const std::string& command, uint16_t channelId, const std::vector<std::string>& additionalParams)
{
    std::string queryCommand = command + queryCharacter;
    return buildCommand(queryCommand, channelId, additionalParams);
}

std::string MessageBuilder::buildQuery(const std::string& command, uint16_t channelId, SC2470::Path path, const std::vector<std::string>& additionalParams)
{
    std::string queryCommand = command + queryCharacter;
    return buildCommand(queryCommand, channelId, path, additionalParams);
}

std::string MessageBuilder::buildQuery(const std::string& command, uint16_t channelId, SC2470::Path path)
{
    std::string queryCommand = command + queryCharacter;
    return buildCommand(queryCommand, channelId, path);
}

std::string MessageBuilder::buildQuery(const std::string& command, uint16_t channelId)
{
    std::string queryCommand = command + queryCharacter;
    return buildCommand(queryCommand, channelId);
}

} // namespace fesd