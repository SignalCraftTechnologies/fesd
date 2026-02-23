#include <fesd/version.hpp>

namespace fesd
{

FESD_API std::string getVersion(void)
{
    return std::string(std::to_string(version::major) + "." + std::to_string(version::minor) + "." + std::to_string(version::point));
}

FESD_API uint16_t getVersionMajor(void)
{
    return version::major;
}

FESD_API uint16_t getVersionMin(void)
{
    return version::minor;
}

FESD_API uint16_t getVersionPoint(void)
{
    return version::point;
}

} // namespace fesd
