#pragma once

#include <fesd/fesd.hpp>

#include <string>
#include <vector>

namespace fesd
{
class MessageBuilder
{
public:
    MessageBuilder(void){};
public:
    static std::string buildCommand(const std::string& command, uint16_t channelId, const std::vector<std::string>& additionalParams);
    static std::string buildCommand(const std::string& command, uint16_t channelId, SC2470::Path path, const std::vector<std::string>& additionalParams);
    static std::string buildCommand(const std::string& command, uint16_t channelId, SC2470::Path path);
    static std::string buildCommand(const std::string& command, uint16_t channelId);
    static std::string buildQuery(const std::string& command, uint16_t channelId, const std::vector<std::string>& additionalParams);
    static std::string buildQuery(const std::string& command, uint16_t channelId, SC2470::Path path, const std::vector<std::string>& additionalParams);
    static std::string buildQuery(const std::string& command, uint16_t channelId, SC2470::Path path);
    static std::string buildQuery(const std::string& command, uint16_t channelId);
};
} // namespace fesd