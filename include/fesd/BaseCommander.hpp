#pragma once

#include <fesd/types/Common.hpp>
#include <fesd/config.h>
#include <cstdint>
#include <memory>
#include <string>

namespace fesd {

class DeviceConnection;
struct DeviceDetails;
class FESD_API GeneralProcessor;

class FESD_API BaseCommander
{
protected:
    BaseCommander(std::shared_ptr<DeviceDetails> device);
    BaseCommander(std::shared_ptr<DeviceConnection> driverSession, uint16_t devId, DeviceType type);

public:
    virtual ~BaseCommander() = default;

public:
    DeviceType getDeviceType(void) const;
    uint16_t getSlotId(void) const;
    void resetDevice(void) const;
    std::string getSerialNumber(void) const;
    std::string getFirmwareVersion(void) const;
    SystemRole getSystemRole(void) const;

protected:
#pragma warning(push) 
#pragma warning(disable:4251)
    std::shared_ptr<GeneralProcessor> m_genProcessor;
#pragma warning(pop)    
};

} // namespace fesd