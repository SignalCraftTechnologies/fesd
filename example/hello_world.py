import pyfesd
    
# Main
if __name__ == "__main__": 
    print("Front-End Serial Driver version: " + pyfesd.version())

    # Example inputs
    ports = "COM6, COM14"
    desiredSlotId = 1 # 0 or 1

    # Start FE serial driver session on specified COM port
    session = pyfesd.FESerialDriver(ports)

    # Find FESD Device with specified parameters
    devices = session.getDevices()
    print("Found " + str(len(devices)) + "FESD devices using ports " + str(ports))
    device = next((d for d in devices if (d.slotId == desiredSlotId and d.type == pyfesd.DeviceType.SC2470)), None)
    if not device:
        print("Error: Could not find SC2470 with slotID = " + str(desiredSlotId) + " using ports " + str(ports))
        exit(1)
    print("Found device SN# " + str(device.serialNumber) + " at slot " + str(device.slotId))

    # Get SC2470 Commander for found device
    sc2470Commander = session.getSC2470Commander(device.serialNumber)

    # Perform example operations using Commander and print results
    print("Setting Frequencies using RFIF on RX path")
    freqs = sc2470Commander.configureFrequencies(pyfesd.SC2470Path.RX, pyfesd.SC2470RfFrequency(12.7E9) , pyfesd.SC2470IfFrequency(6E9))
    print("rfHz=" + str(freqs.rfHz) + " ifHz=" + str(freqs.ifHz) + " loHz=" + str(freqs.loHz))

    print("Setting Gain to minimum on RX path")
    limits = sc2470Commander.getGainLimits(pyfesd.SC2470Path.RX)
    gain = sc2470Commander.configureGain(pyfesd.SC2470Path.RX, limits.minDb)
    print("gainDb=" + str(gain))