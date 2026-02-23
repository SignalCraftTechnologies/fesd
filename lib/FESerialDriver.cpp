#include <fesd/FESerialDriver.hpp>
#include <fesd/types/Exception.hpp>

#include "DeviceConnection.hpp"
#include "GeneralProcessor.hpp"

#include <boost/algorithm/string.hpp>
#include <stdexcept>

namespace {

const uint16_t maxSlotId = 1;

} // static namespace

namespace fesd {

FESerialDriver::FESerialDriver(const std::string& ports)
{
    AddPorts(ports);
}

void FESerialDriver::AddPorts(const std::string& ports)
{
    std::vector<std::string> splitResult;

    boost::split(splitResult, ports, boost::is_any_of(","));

    for (auto& port : splitResult)
    {
        std::shared_ptr<DeviceConnection> conn = DeviceConnection::make(boost::trim_left_copy(port));

        // Check if any devices are on the port, VER command requires no device id
        try
        {
            conn->transact("VER");
        }
        catch(...)
        {
            continue;
        }

        for (uint16_t slotId = 0; slotId <= maxSlotId; slotId++)
        {
            std::shared_ptr<DeviceDetails> device = std::make_shared<DeviceDetails>(conn, slotId);
            fesd::GeneralProcessor processor(device);

            try
            {
                device->type = processor.getDeviceType();
                if (device->type != DeviceType::Undefined)
                {
                    device->serialNumber = processor.getSerialNumberConverted();
                    device->serialNumberStr = processor.getSerialNumber();
                    device->firmwareVersion = processor.getFwVersionConverted();
                    device->hardwareVersion = processor.getHwVersionConverted();
                    m_deviceMap.emplace(processor.getSerialNumber(), device);
                    break;
                }
            }
            catch(...)
            {
                continue;
            }
        }
    }
}

[[nodiscard]] std::vector<FEDevice> FESerialDriver::getDevices(void) const
{
    std::vector<FEDevice> results;

    for(const auto& [serialNumber, device] : m_deviceMap)
    {
        results.push_back({device->slotId, device->type, device->serialNumber, device->serialNumberStr, device->firmwareVersion, device->hardwareVersion});
    }

    return results;
}

[[nodiscard]] SC2470Commander FESerialDriver::getSC2470Commander(const std::string& serialNumber) const
{
    std::shared_ptr<DeviceDetails> device;
    try
    {
        device = m_deviceMap.at(serialNumber);
        if (device->type == DeviceType::SC2470)
            return {device};
    }
    catch(std::out_of_range)
    {       
        throw InvalidArgumentsError(std::string("Could not find a device with serial number " + serialNumber));
    }
    throw InvalidArgumentsError(std::string("Serial number " + serialNumber + " is not a SC2470 device"));
}

[[nodiscard]] SC2470Commander FESerialDriver::getSC2470Commander(uint16_t slotId) const
{
    for(const auto& [serialNumber, device] : m_deviceMap)
    {
        if (device->slotId == slotId && device->type == DeviceType::SC2470)
            return {device};
    }
    throw InvalidArgumentsError(std::string("Could not find any SC2470 devices using slot " + slotId));
}

[[nodiscard]] SC2470Commander FESerialDriver::getSC2470Commander(const FEDevice& device) const
{
    try
    {
        return {m_deviceMap.at(device.serialNumberStr)};
    }
    catch(std::out_of_range)
    {       
        throw InvalidArgumentsError(std::string("Could not find a device with serial number " + device.serialNumber));
    }
}

[[nodiscard]] std::vector<SC2470Commander> FESerialDriver::getSC2470Commanders() const
{
    std::vector<SC2470Commander> results = {};

    for(const auto& [serialNumber, device] : m_deviceMap)
    {
        if (device->type == DeviceType::SC2470)
            results.push_back({device});
    }

    return results;
}

} // namespace fesd