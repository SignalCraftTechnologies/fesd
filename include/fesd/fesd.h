#ifndef FESD_H
#define FESD_H

#include <stdint.h>
#include <stdbool.h>

#include <fesd/config.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum 
    { 
        FESD_CODES_SUCCESS      = 0,
        FESD_CODES_INVALID_ARGS = -1,
        FESD_CODES_COMMS_ERR    = -2,
        FESD_CODES_CALIBRATION_ERR = -3
    } FESD_Codes_t;

    typedef enum
    {
        FESD_SYSTEM_ROLE_CONTROLLER = 0,
        FESD_SYSTEM_ROLE_PERIPHERAL = 1
    } FESD_SystemRole_t;

    typedef enum
    {
        FESD_PATH_RX    = 0,
        FESD_PATH_TX    = 1,
    } FESD_Path_t;

    typedef enum
    {
        FESD_DUPLEX_SETTING_FDD     = 0,
        FESD_DUPLEX_SETTING_TDD_RX  = 1,
        FESD_DUPLEX_SETTING_TDD_TX  = 2,
    } FESD_DuplexSetting_t;

    typedef enum
    {
        FESD_INTERNAL_REF_FREQ_AUTO     = 0,
        FESD_INTERNAL_REF_FREQ_100MHZ   = 1,
        FESD_INTERNAL_REF_FREQ_105MHZ   = 2,
    } FESD_InternalReferenceFrequency_t;

    typedef enum
    {
        FESD_DEV_TYPE_UNDEFINED = 0,
        FESD_DEV_TYPE_SC2470    = 1,
    } FESD_DeviceType_t;

    typedef enum
    {
        FESD_SC2470_REF_SRC_INTERNAL        = 0,
        FESD_SC2470_REF_SRC_EXTERNAL_10MHZ  = 1,
        FESD_SC2470_REF_SRC_EXTERNAL_100MHZ = 2,
    } FESD_SC2470ReferenceSource_t;

    typedef enum
    {
        FESD_SC2470_SYNTH_MODE_INTEGER      = 0,
        FESD_SC2470_SYNTH_MODE_FRACTIONAL   = 1,
    } FESD_SC2470SynthesizerMode_t;

    typedef void* SessionRef_t;
    typedef void* DeviceRef_t;

    FESD_API int16_t FESD_Version(char* result, uint16_t* size);

    FESD_API int16_t FESD_Initialize(const char* serialPort, SessionRef_t* session);
    FESD_API int16_t FESD_DeInitialize(SessionRef_t session);
    FESD_API int16_t FESD_GetDeviceCount(SessionRef_t session, uint16_t* count);
    FESD_API int16_t FESD_GetDevices(SessionRef_t session, uint16_t size, uint16_t* slotIDs, FESD_DeviceType_t* types, uint32_t* serialNumbers, double* firmwareVersions, double* hardwareVersions);
    FESD_API int16_t FESD_SendDirectCommand(SessionRef_t session, char* command, char* result, uint16_t* size);
    FESD_API int16_t FESD_InitializeSC2470Commander(SessionRef_t session, uint32_t serialNumber, DeviceRef_t* sc2470Ref);

    FESD_API int16_t FESD_GetId(DeviceRef_t device, uint16_t* id);
    FESD_API int16_t FESD_ResetDevice(DeviceRef_t device);
    FESD_API int16_t FESD_GetSerialNumber(DeviceRef_t device, char* serialNumber, uint16_t* size);
    FESD_API int16_t FESD_GetFirmwareVersion(DeviceRef_t device, char* firmwareVersion, uint16_t* size);
    FESD_API int16_t FESD_GetSystemRole(DeviceRef_t device, FESD_SystemRole_t* systemRole);

    FESD_API int16_t FESD_SC2470ConfigureGain(DeviceRef_t device, FESD_Path_t path, double* gain);
    FESD_API int16_t FESD_SC2470ConfigureAttenuation(DeviceRef_t device, FESD_Path_t path, double* attn);
    FESD_API int16_t FESD_SC2470ConfigureFrequenciesRfIf(DeviceRef_t device, FESD_Path_t path, double* rfHz, double* ifHz);
    FESD_API int16_t FESD_SC2470ConfigureFrequenciesRfLo(DeviceRef_t device, FESD_Path_t path, double* rfHz, double* loHz);
    FESD_API int16_t FESD_SC2470ConfigureFrequenciesIfLo(DeviceRef_t device, FESD_Path_t path, double* ifHz, double* loHz);
    FESD_API int16_t FESD_SC2470ConfigureFrequencies(DeviceRef_t device, FESD_Path_t path, double* rfHz, double* ifHz, double* loHz);
    FESD_API int16_t FESD_SC2470ConfigureBypassFrequency(DeviceRef_t device, FESD_Path_t path, double* byHz);
    FESD_API int16_t FESD_SC2470ConfigureLoOutput(DeviceRef_t device, FESD_Path_t path, bool* enable1x, bool* enable2x, uint16_t* powerLevel1x, uint16_t* powerLevel2x);
    FESD_API int16_t FESD_SC2470ConfigureLoFrequency(DeviceRef_t device, FESD_Path_t path, double* freqHz);
    FESD_API int16_t FESD_SC2470ConfigureDuplexSetting(DeviceRef_t device, FESD_DuplexSetting_t* setting);
    FESD_API int16_t FESD_SC2470ConfigureReferenceSource(DeviceRef_t device, FESD_SC2470ReferenceSource_t* source);
    FESD_API int16_t FESD_SC2470ConfigureInternalReferenceOverride(DeviceRef_t device, FESD_Path_t path, FESD_InternalReferenceFrequency_t* freqSel);
    FESD_API int16_t FESD_SC2470ConfigurePhaseOffset(DeviceRef_t device, FESD_Path_t path, double* offset);
    FESD_API int16_t FESD_SC2470ConfigureDCBias(DeviceRef_t device, FESD_Path_t path, int16_t* iBias,  int16_t* qBias);
    FESD_API int16_t FESD_SC2470ConfigureReferenceOutputEnable(DeviceRef_t device, bool* enable);
    FESD_API int16_t FESD_SC2470ConfigureLoEnable(DeviceRef_t device, FESD_Path_t path, bool* enable);

    FESD_API int16_t FESD_SC2470GetGain(DeviceRef_t device, FESD_Path_t path, double* gainDb);
    FESD_API int16_t FESD_SC2470GetGainLimits(DeviceRef_t device, FESD_Path_t path, double* minGainDb, double* maxGainDb);
    FESD_API int16_t FESD_SC2470GetAttenuation(DeviceRef_t device, FESD_Path_t path, double* attn);
    FESD_API int16_t FESD_SC2470GetFrequencies(DeviceRef_t device, FESD_Path_t path, double* rfHz, double* ifHz, double* loHz);
    FESD_API int16_t FESD_SC2470GetLoOutput(DeviceRef_t device, FESD_Path_t path, bool* enable1x, bool* enable2x, uint16_t* powerLevel1x, uint16_t* powerLevel2x);
    FESD_API int16_t FESD_SC2470GetLoFrequency(DeviceRef_t device, FESD_Path_t path, double* freqHz);
    FESD_API int16_t FESD_SC2470GetDuplexSetting(DeviceRef_t device, FESD_DuplexSetting_t* setting);
    FESD_API int16_t FESD_SC2470GetReferenceSource(DeviceRef_t device, FESD_SC2470ReferenceSource_t* source);
    FESD_API int16_t FESD_SC2470GetInternalReferenceOverride(DeviceRef_t device, FESD_InternalReferenceFrequency_t* freqSel);
    FESD_API int16_t FESD_SC2470GetSynthesizerMode(DeviceRef_t device, FESD_Path_t path, FESD_SC2470SynthesizerMode_t* mode);
    FESD_API int16_t FESD_SC2470GetPhaseOffset(DeviceRef_t device, FESD_Path_t path, double* offset);
    FESD_API int16_t FESD_SC2470GetDCBias(DeviceRef_t device, FESD_Path_t path, int16_t* iBias,  int16_t* qBias);
    FESD_API int16_t FESD_SC2470GetReferenceOutputEnable(DeviceRef_t device, bool* enable);
    FESD_API int16_t FESD_SC2470GetLoEnable(DeviceRef_t device, FESD_Path_t path, bool* enable);

    
#ifdef __cplusplus
} // extern C
#endif

#endif // FESD_H

