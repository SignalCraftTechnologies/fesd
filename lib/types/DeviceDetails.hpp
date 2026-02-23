#pragma once

#include <fesd/fesd.hpp>

#include <cstdint>
#include <memory>

namespace fesd {

class DeviceConnection;

struct DeviceDetails
{
    std::shared_ptr<DeviceConnection> connection;
    uint16_t slotId;
    DeviceType type;
    uint32_t serialNumber;
    std::string serialNumberStr;
    double firmwareVersion;
    double hardwareVersion;
};

} // namespace fesd
