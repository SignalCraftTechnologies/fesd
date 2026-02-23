#pragma once

#include <fesd/types/Common.hpp>
#include <memory>
#include <string>
#include <map>

namespace fesd {

class BaseCommander;

class DeviceConnection final
{
public:
    using sptr = std::shared_ptr<DeviceConnection>;
    [[nodiscard]] static sptr make(std::string port);
    ~DeviceConnection();
    std::string transact(const std::string& message) const;
    void resetConnection(const std::string& notifyMessage) const;

private:
    struct Detail;
    std::unique_ptr<Detail> m_detail;
    DeviceConnection(std::string port);
};

} // namespace fesd