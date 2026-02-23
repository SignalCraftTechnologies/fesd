#include <fesd/fesd.hpp>
#include <iostream>
#include <vector>
#include <string>

int main()
{
    /******** SETUP ********/
    // Example inputs
    std::string port = "COM6";
    // Start FE serial driver session on specified COM port
    auto fesd = fesd::FESerialDriver(port);

    auto devices = fesd.getDevices();
    auto device = devices[0];
    std::cout << device.serialNumber << " " << device.slotId << std::endl;
    fesd::SC2470Commander commander = fesd.getSC2470Commander(device.slotId);
    fesd::SC2470::FrequencySet freqs;
    std::cout << "RFIF" << std::endl;
    freqs = commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::RfFrequency(12.7E9), fesd::SC2470::IfFrequency(6e9));
    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;

    return 0;
}