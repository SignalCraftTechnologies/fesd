#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <fesd/fesd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAXDEVICES 10U
#define MAXSTRINGLENGTH 20U

int main() {

    // Example inputs
    char* ports = "COM6,COM14";
    uint16_t desiredSlotId = 1;

    char stringBuffer[MAXSTRINGLENGTH];
    uint16_t stringSize = MAXSTRINGLENGTH;

    if(FESD_Version(stringBuffer, &stringSize)) {
        exit(1);
    }
    fprintf(stderr, "FESD Version: %s \r\n", stringBuffer);

    // Start FE serial driver session on specified COM ports
    SessionRef_t session = 0;
    if(FESD_Initialize(ports, &session)) {
        exit(1);
    }

    // Find FESD Device with specified parameters
    uint16_t deviceCount = 0;
    if(FESD_GetDeviceCount(session, &deviceCount)) {
        exit(1);
    }
    fprintf(stderr, "Found %u FESD devices using ports %s \r\n", deviceCount, ports);

    uint16_t deviceSlotIDs[MAXDEVICES];
    FESD_DeviceType_t deviceTypes[MAXDEVICES];
    uint32_t deviceSerialNumbers[MAXDEVICES];
    double deviceFirmwareVersions[MAXDEVICES];
    double deviceHardwareVersions[MAXDEVICES];
    if(FESD_GetDevices(session, deviceCount, deviceSlotIDs, deviceTypes, deviceSerialNumbers, deviceFirmwareVersions, deviceHardwareVersions)) {
        exit(1);
    }

    uint32_t sc2470SerialNumber = 0;
    for(int i=0; i< deviceCount; i++) {
        if(deviceSlotIDs[i] == desiredSlotId && deviceTypes[i] == FESD_DEV_TYPE_SC2470) {
            sc2470SerialNumber = deviceSerialNumbers[i];
            break;
        }
    }
    if(!sc2470SerialNumber) {
        fprintf(stderr, "Error: Could not find SC2470 with desired slotID from provided ports \r\n");
        exit(1);
    }
    fprintf(stderr, "Found SC2470 with serial number %u\r\n", sc2470SerialNumber);

    // Get SC2470 Commander for found device
    uint64_t sc2470Ref_placeholder = 0;
    DeviceRef_t sc2470Ref = &sc2470Ref_placeholder;
    if(FESD_InitializeSC2470Commander(session, sc2470SerialNumber, &sc2470Ref)) {
        exit(1);
    }

    // Cleanup
    FESD_DeInitialize(session);
    return 0;
}
    
#ifdef __cplusplus
} // extern C
#endif

