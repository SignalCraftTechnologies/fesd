#include <fesd/BaseCommander.hpp>

#include "types/DeviceDetails.hpp"
#include "GeneralProcessor.hpp"
#include "DeviceConnection.hpp"

namespace {

const uint16_t devNumMin = 1;
const uint16_t devNumMax = 4;

} // static namespace

namespace fesd {

BaseCommander::BaseCommander(std::shared_ptr<DeviceConnection> driverSession, uint16_t devId, DeviceType type)
    : m_genProcessor(std::make_shared<GeneralProcessor>(std::make_shared<DeviceDetails>(driverSession, devId, type)))
{

}
BaseCommander::BaseCommander(std::shared_ptr<DeviceDetails> details)
    : m_genProcessor(std::make_shared<GeneralProcessor>(details))
{
}

DeviceType BaseCommander::getDeviceType(void) const
{
    return m_genProcessor->getDeviceDetails()->type;
}

uint16_t BaseCommander::getSlotId(void) const
{
    return m_genProcessor->getDeviceDetails()->slotId;
}

void BaseCommander::resetDevice(void) const
{
    return m_genProcessor->resetDevice();
}

std::string BaseCommander::getSerialNumber(void) const
{
    return m_genProcessor->getSerialNumber();
}

std::string BaseCommander::getFirmwareVersion(void) const
{
    return m_genProcessor->getFwVersion();
}

SystemRole BaseCommander::getSystemRole(void) const
{
    return m_genProcessor->getSystemRole();
}

} // namespace fesd

