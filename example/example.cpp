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
    freqs = commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::RfFrequency(6.7E9), fesd::SC2470::IfFrequency(0.7e9));
    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;

    freqs = commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::RfFrequency(26E9), fesd::SC2470::IfFrequency(0.7e9));
    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;

    freqs = commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::RfFrequency(26E9), fesd::SC2470::IfFrequency(6e9));
    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;

    std::cout << "RFLO" << std::endl;
    freqs = commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::RfFrequency(6.7E9), fesd::SC2470::LoFrequency(6e9));
    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;
    freqs = commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::RfFrequency(12E9), fesd::SC2470::LoFrequency(6e9));
    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;
    freqs = commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::RfFrequency(26E9), fesd::SC2470::LoFrequency(20e9));
    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;

    freqs = commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::RfFrequency(26E9), fesd::SC2470::LoFrequency(25.3e9));
    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;

    std::cout << "IFLO" << std::endl;
    freqs = commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::IfFrequency(0.7E9), fesd::SC2470::LoFrequency(6e9));
    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;

    freqs = commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::IfFrequency(6E9), fesd::SC2470::LoFrequency(6e9));
    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;
    freqs = commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::IfFrequency(0.7e9), fesd::SC2470::LoFrequency(25.3e9));
    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;

    std::cout << freqs.rfHz << " " << freqs.ifHz << " " << freqs.loHz << std::endl;

    commander.configureFrequencies(fesd::SC2470::Path::RX, {11.755E9,3.255E9,0});
    freqs = commander.getFrequencies(fesd::SC2470::Path::RX);

    commander.configureDCBias(fesd::SC2470::Path::TX, fesd::SC2470::DCBias(2000, 1000));
    auto dcBias = commander.getDCBias(fesd::SC2470::Path::TX);

    fesd::SC2470::GainLimitsSet limits = commander.getGainLimits(fesd::SC2470::Path::TX);
    auto gain = commander.configureGain(fesd::SC2470::Path::TX, limits.minDb);

    std::cout << "Set gain to " << gain << "dB" << std::endl;

    return 0;
}