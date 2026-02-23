#include "DeviceConnection.hpp"
#include <fesd/SC2470Commander.hpp>
#include "SerialConsole.hpp"
#include <mutex>
#include <chrono>
#include <thread>

namespace fesd {

struct DeviceConnection::Detail {
    SerialConsole serial;
    std::mutex mutex;
};

[[nodiscard]] DeviceConnection::sptr DeviceConnection::make(std::string port)
{
    return std::shared_ptr<DeviceConnection>(new DeviceConnection(port));
}

DeviceConnection::DeviceConnection(std::string port)
    : m_detail(std::make_unique<Detail>(port))
{
}
DeviceConnection::~DeviceConnection() = default;

std::string DeviceConnection::transact(const std::string& message) const
{
    std::scoped_lock<std::mutex> lock(m_detail->mutex);
    return m_detail->serial.transact(message);
}

void DeviceConnection::resetConnection(const std::string& notifyMessage) const
{
    std::scoped_lock<std::mutex> lock(m_detail->mutex);

    if (notifyMessage.length() > 0)
        m_detail->serial.write(notifyMessage);
    m_detail->serial.disconnect();
    // 5s was not enough to account for NVM config load after device startup, 10s works
    std::this_thread::sleep_for(std::chrono::seconds(10));
    m_detail->serial.reconnect();
}

} // namespace fesd
