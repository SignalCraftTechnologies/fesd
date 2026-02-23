#include "GeneralProcessor.hpp"
#include "MessageBuilder.hpp"
#include "DeviceConnection.hpp"
#include <fesd/types/Exception.hpp>

#include <boost/algorithm/string.hpp>
#include <stdexcept>
#include <map>

namespace {
// Commands
const std::string reset = "*RST";
const std::string identification = "*IDN";
const std::string getManuf = "MAINT:GETMANUF";
const std::string sysRole = "SYS:ROLE";
const std::string sysNumChannels = "SYS:NCHAN";

const std::map<fesd::SystemRole, std::string> SystemRoleStringMap = {
    {fesd::SystemRole::Controller,      "MASTER"},
    {fesd::SystemRole::Peripheral,      "SLAVE"},
};

const std::map<fesd::DeviceType, std::string> DeviceTypeStringMap = {
    {fesd::DeviceType::SC2470,          "SC2470"},
};

std::string returnSplitString(const std::string& message, const std::string& delim, uint16_t desiredIndex)
{
    std::vector<std::string> splitResult;

    boost::split(splitResult, message, boost::is_any_of(delim));

    if (desiredIndex < splitResult.size())
        return boost::trim_left_copy(splitResult[desiredIndex]);
    return "";
}
} // static namespace


namespace fesd {

void GeneralProcessor::resetDevice() const
{
    m_details->connection->resetConnection(MessageBuilder::buildCommand(reset, m_details->slotId));
    // Verify connection
    this->getId();
}

std::string GeneralProcessor::getId() const
{
    return m_details->connection->transact(MessageBuilder::buildQuery(identification, m_details->slotId));
}

std::string GeneralProcessor::getSerialNumber() const
{
    const std::string delim(" ");
    // index 0
    const std::string result = returnSplitString(m_details->connection->transact(MessageBuilder::buildQuery(getManuf, m_details->slotId)), delim, 0);
    return boost::trim_left_copy_if(result, boost::is_any_of("#H"));
}

uint32_t GeneralProcessor::getSerialNumberConverted() const
{
    return std::stoul(this->getSerialNumber(), nullptr, 16);
}

std::string GeneralProcessor::getHwVersion() const
{
    const std::string delim(" ");
    // index 2
    return returnSplitString(m_details->connection->transact(MessageBuilder::buildQuery(getManuf, m_details->slotId)), delim, 2);
}

double GeneralProcessor::getHwVersionConverted() const
{
    return std::stod(this->getHwVersion());
}

DeviceType GeneralProcessor::getDeviceType() const
{
    const std::string delim(",");
    // index 1
    const std::string result = returnSplitString(m_details->connection->transact(MessageBuilder::buildQuery(identification, m_details->slotId)), delim, 1);

    for(const auto& [key, value] : DeviceTypeStringMap)
    {
        if (result.compare(value) == 0)
        {
            return key;
        }
    }
    return DeviceType::Undefined;
}

std::string GeneralProcessor::getFwVersion() const
{
    const std::string delim(",");
    // index 3
    return returnSplitString(m_details->connection->transact(MessageBuilder::buildQuery(identification, m_details->slotId)), delim, 3);
}

double GeneralProcessor::getFwVersionConverted() const
{
    return std::stod(this->getFwVersion());
}

SystemRole GeneralProcessor::getSystemRole() const
{
    std::string result = m_details->connection->transact(MessageBuilder::buildQuery(sysRole, m_details->slotId));

    for(const auto& [key, value] : SystemRoleStringMap)
    {
        if (result.compare(value) == 0)
        {
            return key;
        }
    }
    throw CommunicationError("Invalid response from device...");
}

void GeneralProcessor::setSystemRole(SystemRole role) const
{
    std::vector<std::string> params {SystemRoleStringMap.at(role)};
    m_details->connection->transact(MessageBuilder::buildCommand(sysRole, m_details->slotId, params));
}

uint16_t GeneralProcessor::getNumberOfDevices() const
{
    return static_cast<uint16_t>(std::stoul(m_details->connection->transact(MessageBuilder::buildQuery(sysNumChannels, m_details->slotId))));
}

std::shared_ptr<DeviceDetails> GeneralProcessor::getDeviceDetails() const
{
    return m_details;
}

} // namespace fesd