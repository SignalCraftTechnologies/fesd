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

#define THROW_IF_NONZERO(expr)                      \
    do {                                            \
        int rc = (expr);                            \
        if (rc != 0) {                              \
            fprintf(stderr,                         \
                    "Error: %s failed (%d)\n",      \
                    #expr, rc);                     \
            exit(EXIT_FAILURE);                     \
        }                                           \
    } while (0)

struct StringDetails {
    char buffer[MAXSTRINGLENGTH];
    uint16_t size;
};

struct DoubleDetails {
    double set;
    double get;
};

#define PRINTF(...) \
    do { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\r\n"); } while (0)

#define MAKESTRING(VAR) \
    struct StringDetails VAR = { .size = MAXSTRINGLENGTH }

#define PRINTSTRING(VAR)                                      \
    PRINTF(#VAR "[%u]: %s", (VAR).size, (VAR).buffer);    \

inline static sep(void) {
    PRINTF("################################");
}

int main() {

    char* port = "COM6,COM14";
    sep();
    MAKESTRING(fesdVersion);
    THROW_IF_NONZERO(FESD_Version(fesdVersion.buffer, &fesdVersion.size));
    PRINTSTRING(fesdVersion);
    sep();

    SessionRef_t session = 0;
    THROW_IF_NONZERO(FESD_Initialize(port, &session));
    uint16_t deviceCount = 0;
    FESD_GetDeviceCount(session, &deviceCount);
    PRINTF("Found %u FESD devices", deviceCount);
    sep();

    uint16_t deviceSlotIDs[MAXDEVICES];
    FESD_DeviceType_t deviceTypes[MAXDEVICES];
    uint32_t deviceSerialNumbers[MAXDEVICES];
    double deviceFirmwareVersions[MAXDEVICES];
    double deviceHardwareVersions[MAXDEVICES];
    THROW_IF_NONZERO(FESD_GetDevices(session, deviceCount, deviceSlotIDs, deviceTypes, deviceSerialNumbers, deviceFirmwareVersions, deviceHardwareVersions));

    uint16_t sc2470Count = 0;
    uint16_t sc2470SlotIDs[MAXDEVICES];
    uint32_t sc2470SerialNumbers[MAXDEVICES];
    double sc2470FirmwareVersions[MAXDEVICES];
    double sc2470HardwareVersions[MAXDEVICES];
    for(int i=0; i< deviceCount; i++) {
        if( deviceTypes[i] == FESD_DEV_TYPE_SC2470) {
            sc2470SlotIDs[sc2470Count] = deviceSlotIDs[i];
            sc2470SerialNumbers[sc2470Count] = deviceSerialNumbers[i];
            sc2470FirmwareVersions[sc2470Count] = deviceFirmwareVersions[i];
            sc2470HardwareVersions[sc2470Count] = deviceHardwareVersions[i];
            sc2470Count += 1;
        }
    }

    PRINTF("Found %u SC2470 devices", sc2470Count);
    for(int i=0;i<sc2470Count;i++) {

        PRINTF("SC2470 #%u", i);
        PRINTF("Slot ID: %u", sc2470SlotIDs[i]);
        PRINTF("Serial Number: %04X", sc2470SerialNumbers[i]);
        PRINTF("Firmware Version: %lf", sc2470FirmwareVersions[i]);
        PRINTF("Hardware Version: %lf", sc2470HardwareVersions[i]);
    }
    sep();

    PRINTF("Initializing a SC2470 device...");
    uint64_t sc2470Ref_placeholder = 0;
    DeviceRef_t sc2470Ref = &sc2470Ref_placeholder;
    THROW_IF_NONZERO(FESD_InitializeSC2470Commander(session, sc2470SerialNumbers[0], &sc2470Ref));
    uint16_t id = 0;
    THROW_IF_NONZERO(FESD_GetId(sc2470Ref, &id));
    PRINTF("slotId: %d", id);

    MAKESTRING(serialNumber);
    THROW_IF_NONZERO(FESD_GetSerialNumber(sc2470Ref, serialNumber.buffer, &serialNumber.size));
    PRINTSTRING(serialNumber);

    MAKESTRING(firmwareVersion);
    THROW_IF_NONZERO(FESD_GetFirmwareVersion(sc2470Ref, firmwareVersion.buffer, &firmwareVersion.size));
    PRINTSTRING(firmwareVersion);

    FESD_SystemRole_t systemRole = 0;
    THROW_IF_NONZERO(FESD_GetSystemRole(sc2470Ref, &systemRole));
    bool isController = systemRole == FESD_SYSTEM_ROLE_CONTROLLER;
    PRINTF("SystemRole: %s", systemRole ? "Peripheral" : "Controller");
    sep();

    double ifHz = 3e9;
    double loHz = 12e9;
    PRINTF("IFLO Setting Frequencies configured %lf and %lf", ifHz, loHz);
    THROW_IF_NONZERO(FESD_SC2470ConfigureFrequenciesIfLo(sc2470Ref, FESD_PATH_RX, &ifHz, &loHz));
    PRINTF("IFLO Setting Frequencies read back  %lf and %lf", ifHz, loHz);
    int16_t iBias = 2047;
    int16_t qBias = 2047;
    THROW_IF_NONZERO(FESD_SC2470ConfigureDCBias(sc2470Ref, FESD_PATH_TX, &iBias, &qBias));
    PRINTF("BIAS  %u and %u", iBias, qBias);

    FESD_DeInitialize(session);
    return 0;
}
    
#ifdef __cplusplus
} // extern C
#endif

