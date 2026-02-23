#pragma once

#include <fesd/config.h>
#include <fesd/SC2470Commander.hpp>

#include <string>
#include <memory>
#include <cstdint>
#include <vector>

namespace fesd
{

class DeviceConnection;

struct FEDevice
{
    uint16_t slotId;
    DeviceType type;
    uint32_t serialNumber;
    std::string serialNumberStr;
    double firmwareVersion;
    double hardwareVersion;
};

class FESD_API FESerialDriver final
{
public:
    FESerialDriver(const std::string& ports);
public:
    using DeviceMap = std::map<const std::string, std::shared_ptr<DeviceDetails>>;
    void AddPorts(const std::string& ports);
    [[nodiscard]] std::vector<FEDevice> getDevices(void) const;
    [[nodiscard]] SC2470Commander getSC2470Commander(const std::string& serialNumber) const;
    [[nodiscard]] SC2470Commander getSC2470Commander(const uint16_t slotId) const;
    [[nodiscard]] SC2470Commander getSC2470Commander(const FEDevice& device) const;
    [[nodiscard]] std::vector<SC2470Commander> getSC2470Commanders(void) const;

private:
#pragma warning(push) 
#pragma warning(disable:4251)
    DeviceMap m_deviceMap;
#pragma warning(pop)  
};

} // namespace fesd