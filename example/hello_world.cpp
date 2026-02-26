#include <fesd/fesd.hpp>
#include <iostream>
#include <vector>
#include <string>

int main()
{
    // Example inputs
    
    std::string ports = "";
    std::string desiredSlotStr = "";
    std::cout << "SELECT COM PORTS: eg: 'COM6,COM14' or '/dev/ttyUSB0,/dev/ttyUSB1'" << std::endl;
    std::getline(std::cin, ports);
    std::cout << "SELECT DESIRED SLOTID 0/1 eg: '1'" << std::endl;
    std::getline(std::cin, desiredSlotStr);

    uint16_t desiredSlotId = desiredSlotStr == "1" ? 1 : 0;

    // Start FE serial driver session on specified COM port
    fesd::FESerialDriver fesd = fesd::FESerialDriver(ports);

    // Find FESD Device with specified parameters
    std::vector<fesd::FEDevice> devices = fesd.getDevices();
     std::cout << "Found " << devices.size() << "FESD devices using ports " << ports << std::endl;
    fesd::FEDevice device;
    bool foundDevice = false;
    for(fesd::FEDevice dev : devices) {
        if(dev.slotId == desiredSlotId && dev.type == fesd::DeviceType::SC2470) {
            device = dev;
            foundDevice = true;
            break;
        }
    }
    if(!foundDevice) {
        std::cout << "Error: Could not find SC2470 with slotID = " << desiredSlotId << " using ports " << ports << std::endl;
        exit(1);
    }
    std::cout << "Found device SN# " << device.serialNumberStr << " at slot " << device.slotId << std::endl;

    // Get SC2470 Commander for found device
    fesd::SC2470Commander sc2470Commander = fesd.getSC2470Commander(device.serialNumberStr);

    // Perform example operations using Commander and print results
    fesd::SC2470::FrequencySet freqs;
    std::cout << "Setting Frequencies using RFIF on RX path" << std::endl;
    freqs = sc2470Commander.configureFrequencies(fesd::SC2470::Path::RX, fesd::SC2470::RfFrequency(12.7E9), fesd::SC2470::IfFrequency(6e9));
    std::cout << "rfHz=" << freqs.rfHz << " ifHz=" << freqs.ifHz << " loHz=" << freqs.loHz << std::endl;

    std::cout << "Setting Gain to minimum on RX path" << std::endl;
    fesd::SC2470::GainLimitsSet limits = sc2470Commander.getGainLimits(fesd::SC2470::Path::RX);
    auto gain = sc2470Commander.configureGain(fesd::SC2470::Path::TX, limits.minDb);
    std::cout << "gainDb=" << gain << std::endl;

    return 0;
}