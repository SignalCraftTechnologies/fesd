#pragma once

#include "types/DeviceDetails.hpp"

#include <fesd/types/Common.hpp>

#include <string>

namespace fesd {

class GeneralProcessor
{
public:
    GeneralProcessor(std::shared_ptr<DeviceDetails> details) : m_details(details) {};

public:
    void resetDevice() const;
    std::string getId() const;
    std::string getSerialNumber() const;
    uint32_t getSerialNumberConverted() const;
    std::string getHwVersion() const;
    double getHwVersionConverted() const;
    std::string getFwVersion() const;
    double getFwVersionConverted() const;
    DeviceType getDeviceType() const;
    void setSystemRole(SystemRole role) const;
    uint16_t getNumberOfDevices() const;
    SystemRole getSystemRole() const;
    std::shared_ptr<DeviceDetails> getDeviceDetails() const;

private:
#pragma warning(push) 
#pragma warning(disable:4251)  
    std::shared_ptr<DeviceDetails> m_details;
#pragma warning(pop) 
};

} // namespace fesd
