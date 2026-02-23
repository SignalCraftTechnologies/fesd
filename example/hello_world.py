import pyfesd
    
# Main
if __name__ == "__main__": 
    print("Front-End Serial Driver version: " + pyfesd.version())

    ### GET SESSION ###
    session = pyfesd.FESerialDriver("COM6, COM14")
    devices = session.getDevices()

    ### GET COMMANDERS ###
    sc2470Slot0 = session.getSC2470Commander(0)

    ### SET FREQ ###
    rfHz = 11.755E9
    ifHz = 3.255E9
    loHz = 0 # Let device set this
    freqs = pyfesd.SC2470FrequencySet(rfHz, ifHz, loHz)
    freqs2 = sc2470Slot0.configureFrequencies(pyfesd.SC2470Path.RX, freqs)
    print("\tFreqs In: " + str(freqs.rfHz) + " " + str(freqs.ifHz) + " " + str(freqs.loHz))
    print("\tFreqs Out: " + str(freqs2.rfHz) + " " + str(freqs2.ifHz) + " " + str(freqs2.loHz))
