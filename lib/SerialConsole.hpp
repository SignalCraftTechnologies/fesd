#pragma once
#include <memory>
#include <string>

namespace fesd {

class SerialConsole final
{
public:
    SerialConsole(std::string device);
    ~SerialConsole();
    std::string transact(const std::string& message) const;
    void write(const std::string& message) const;
    void disconnect(void) const;
    void reconnect(void) const;

private:
    void connect(void) const;
    std::string read(void) const;

private:
    struct Device;
    std::unique_ptr<Device> m_dev;
};

} // namespace rtsd
