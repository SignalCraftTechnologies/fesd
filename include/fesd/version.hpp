#pragma once

#include <string>
#include <cstdint>

#include <fesd/config.h>

namespace fesd 
{

namespace version
{
    const uint16_t major = 0;
    const uint16_t minor = 0;
    const uint16_t point = 7;
}

FESD_API std::string getVersion(void);
FESD_API uint16_t getVersionMajor(void);
FESD_API uint16_t getVersionMin(void);
FESD_API uint16_t getVersionPoint(void);

} // namespace fesd
