#include "SerialConsole.hpp"
#include <fesd/types/Exception.hpp>

#include <regex>
#include <stdexcept>
#ifdef WIN32
#include "wintargetsys.h" // required for boost/asio on windows, include before boost/asio
#endif // WIN32
#include <boost/asio.hpp>
#include <chrono>
#include <iostream>

namespace {
const std::string CommandPrompt = ">";
const std::regex SuccessRegex("(\r|\n)(OK)(\r|\n)\\" + CommandPrompt);
const std::regex ErrorRegex("(\r|\n|)(ERR)(\r|\n)" + CommandPrompt);
const std::regex PeripheralResponseRegex("(\\[ [0-9]+ \\] )");
const uint32_t ReadTimeoutSeconds = 10;

using SerialSetting = boost::asio::serial_port_base;

void InitSerialSettings(boost::asio::serial_port& serial)
{
    serial.set_option(SerialSetting::baud_rate(115200));
    serial.set_option(SerialSetting::parity(SerialSetting::parity::even));
    serial.set_option(SerialSetting::stop_bits(SerialSetting::stop_bits::one));
    serial.set_option(SerialSetting::flow_control(SerialSetting::flow_control::none));
}

} // namespace

namespace fesd {

struct SerialConsole::Device
{
    std::string port;
    boost::asio::io_context io;
    boost::asio::serial_port serial;
    Device(std::string serialPort) : io(), serial(io) 
    {
        port = serialPort;
    }
    ~Device() {}
};

SerialConsole::SerialConsole(std::string device) : m_dev(std::make_unique<Device>(device))
{
    this->connect();
}
SerialConsole::~SerialConsole() = default;

void SerialConsole::connect() const
{
    try
    {
        m_dev->serial.open(m_dev->port);
    }
    catch (...)
    {
        throw CommunicationError(std::string("Failed to open device " + m_dev->port));
    }

    InitSerialSettings(m_dev->serial);

    // Clean out device buffer - ETX(0x03) will force device to clear its buffer
    write("\x03");

    // Clean out PC / return buffers
    try
    {    
        boost::asio::streambuf buffer;
        m_dev->io.restart();
        boost::asio::async_read_until(m_dev->serial, buffer, CommandPrompt, 
            [](const boost::system::error_code& e, std::size_t size) 
            {
                return; // Do nothing with results
            }
        );
            
        m_dev->io.run_for(std::chrono::milliseconds(250)); // Arbitrary time of 250ms
        m_dev->io.restart();
    }
    catch(...)
    {
        throw CommunicationError("Serial communication error...");
    }
}

void SerialConsole::disconnect(void) const
{
    m_dev->serial.cancel();
    m_dev->serial.close();
    m_dev->io.stop();
}

void SerialConsole::reconnect(void) const
{
    this->connect();
}

std::string SerialConsole::transact(const std::string& message) const
{
   write(message);
   std::string response = read();

   if (std::regex_search(response, ErrorRegex))
      throw InvalidArgumentsError("Command Error Returned");

   // Remove any peripheral response overhead, then
   // Remove the leading new lines + status response
   return std::regex_replace(std::regex_replace(response, PeripheralResponseRegex, ""), SuccessRegex, "");
}
void SerialConsole::write(const std::string& message) const
{
    const std::string termination = "\x0D";
    try
    {
        boost::asio::write(m_dev->serial, boost::asio::buffer(message + termination));
    }
    catch(...)
    {
        throw CommunicationError("Serial communication error...");
    }
}

std::string SerialConsole::read(void) const
{
    boost::asio::streambuf buffer;
    std::chrono::time_point readStart = std::chrono::steady_clock::now();

    bool readComplete = false;
    m_dev->io.restart();
    try
    { 
        boost::asio::async_read_until(m_dev->serial, buffer, CommandPrompt, 
            [&readComplete](const boost::system::error_code& e, std::size_t size) 
            {
                if (!e) 
                    readComplete = true;
            }
        );

        while (!readComplete)
        {
            // Boost ASIO async, understanding the basics:
            // https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/overview/core/basics.html
            m_dev->io.run_one_for(std::chrono::seconds(1));
            if ((std::chrono::steady_clock::now() - readStart) > std::chrono::seconds(ReadTimeoutSeconds))
                break;
        }
    }
    catch(...)
    {
        throw CommunicationError("Serial communication error...");
    }

    if (!readComplete) 
        throw CommunicationError("Serial communication timeout...");
    return std::string((std::istreambuf_iterator<char>(&buffer)), std::istreambuf_iterator<char>());
}

} // namespace rtsd
