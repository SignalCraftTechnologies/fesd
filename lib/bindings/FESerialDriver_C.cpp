#include <fesd/fesd.h>
#include <fesd/fesd.hpp>

#include <map>    
#include <cstring>

#define CheckReference(ref)                 \
    if (ref == nullptr)                     \
        return FESD_CODES_INVALID_ARGS;       

#define GetDriverSession(ref, result)       \
    CheckReference(ref);                    \
    result = FESerialDriverCast(ref);       \
    CheckReference(result)

#define GetBaseCommander(ref, result)       \
    CheckReference(ref);                    \
    result = BaseCommanderCast(ref);        \
    CheckReference(result)

#define GetSC2470Commander(ref, result)     \
    CheckReference(ref);                    \
    result = SC2470CommanderCast(ref);      \
    CheckReference(result)

#define FESD_C_CATCH_AND_RETURN(...)        \
    try                                     \
    {                                       \
        __VA_ARGS__                         \
    }                                       \
    catch (fesd::CommunicationError& e)     \
    {                                       \
        (void)e;/*handle compiler warning*/ \
        return FESD_CODES_COMMS_ERR;        \
    }                                       \
    catch (fesd::InvalidArgumentsError& e)  \
    {                                       \
        (void)e;/*handle compiler warning*/ \
        return FESD_CODES_INVALID_ARGS;          \
    }                                       \
    return FESD_CODES_SUCCESS;          

namespace
{
    struct Session
    {
        std::unique_ptr<fesd::FESerialDriver> feSerialDriver;
        std::vector<std::unique_ptr<fesd::BaseCommander>> commanders;
    };

    std::vector<Session> sessions = {};

    inline fesd::FESerialDriver* FESerialDriverCast(void* ptr)
    {
        return reinterpret_cast<fesd::FESerialDriver*>(ptr);
    }

    inline fesd::BaseCommander* BaseCommanderCast(void* ptr)
    {
        return reinterpret_cast<fesd::BaseCommander*>(ptr);
    }

    inline fesd::SC2470Commander* SC2470CommanderCast(void* ptr)
    {
        fesd::BaseCommander* commander = BaseCommanderCast(ptr);
        if (commander->getDeviceType() != fesd::DeviceType::SC2470)
            return nullptr;
        // We are casting to a pointer, on fail it will return nullptr
        return dynamic_cast<fesd::SC2470Commander*>(commander);
    }

    void copyToCStr(char* result, std::string input, uint16_t* size)
    {
        // We need to support GCC and MSVC, can't use strncpy_s
        #pragma warning(push) 
        #pragma warning(disable:4996)
        strncpy(result, input.c_str(), *size);
        #pragma warning(pop)  

        // Check for null char
        if (result[*size - 1] == 0)
        {
            // Update size
            *size = static_cast<uint16_t>(strlen(result));
        }
        else
        {
            // Maxed out, ensure null termination
            result[*size - 1] = 0;
        }
    }

}     

FESD_API int16_t FESD_Version(char* result, uint16_t* size)
{  
    std::string version = fesd::getVersion();

    if (*size < version.length())
        return FESD_CODES_INVALID_ARGS;

    copyToCStr(result, version, size);
    return FESD_CODES_SUCCESS;
}

FESD_API int16_t FESD_Initialize(const char* serialPort, SessionRef_t* session)
{
    FESD_C_CATCH_AND_RETURN
    (
        sessions.push_back({std::make_unique<fesd::FESerialDriver>(std::string(serialPort))});
        *session = sessions.back().feSerialDriver.get();
    )
}

FESD_API int16_t FESD_DeInitialize(SessionRef_t session)
{
    uint16_t index = 0;

    for (Session& s : sessions)
    {
        if (s.feSerialDriver.get() == session)
        {
            for (std::unique_ptr<fesd::BaseCommander>& commanderPtr : s.commanders)
                commanderPtr = nullptr;
            s.commanders.clear();
            s.feSerialDriver = nullptr;
            break;
        }
        index++;
    }

    if (index < sessions.size())
        sessions.erase(sessions.begin() + index);

    return FESD_CODES_SUCCESS;
}

FESD_API int16_t FESD_GetDeviceCount(SessionRef_t session, uint16_t* count)
{
    for (Session& s : sessions)
    {
        if (s.feSerialDriver.get() == session)
        {
            FESD_C_CATCH_AND_RETURN
            (
                
                auto size = s.feSerialDriver->getDevices().size();
                *count = static_cast<uint16_t>(size);
            )
        }
    }
    return FESD_CODES_INVALID_ARGS;
}

FESD_API int16_t FESD_GetDevices(SessionRef_t session, uint16_t size, uint16_t* slotIDs, FESD_DeviceType_t* types, uint32_t* serialNumbers, double* firmwareVersions, double* hardwareVersions)
{
    for (Session& s : sessions)
    {
        if (s.feSerialDriver.get() == session)
        {

            FESD_C_CATCH_AND_RETURN
            (
                std::vector<fesd::FEDevice> devices = s.feSerialDriver->getDevices();
                if (size < devices.size())
                    return FESD_CODES_INVALID_ARGS;

                uint16_t index = 0;
                for (const auto& dev : devices)
                {
                    slotIDs[index] = dev.slotId;
                    types[index] = static_cast<FESD_DeviceType_t>(dev.type);
                    serialNumbers[index] = dev.serialNumber;
                    firmwareVersions[index] = dev.firmwareVersion;
                    hardwareVersions[index] = dev.hardwareVersion;
                    index++;
                }
            )

        }
    }
    return FESD_CODES_INVALID_ARGS;
}

FESD_API int16_t FESD_InitializeSC2470Commander(SessionRef_t session, uint32_t serialNumber, DeviceRef_t* sc2470Ref)
{
    for (Session& s : sessions)
    {

        if (s.feSerialDriver.get() == session) {
            char serialNumberStr[9];
            std::sprintf(serialNumberStr, "%X", serialNumber);

            FESD_C_CATCH_AND_RETURN
            (
                s.commanders.push_back(std::make_unique<fesd::SC2470Commander>(s.feSerialDriver->getSC2470Commander(serialNumberStr)));
                *sc2470Ref = s.commanders.back().get();
            )
        }
    }
    return FESD_CODES_INVALID_ARGS;
}

FESD_API int16_t FESD_GetId(DeviceRef_t device, uint16_t* id)
{
    fesd::BaseCommander* baseDevice;
    GetBaseCommander(device, baseDevice)

    FESD_C_CATCH_AND_RETURN
    (
        *id = baseDevice->getSlotId();
    )
}

FESD_API int16_t FESD_ResetDevice(DeviceRef_t device)
{
    fesd::BaseCommander* baseDevice;
    GetBaseCommander(device, baseDevice)

    FESD_C_CATCH_AND_RETURN
    (
        baseDevice->resetDevice();
    )
}

FESD_API int16_t FESD_GetSerialNumber(DeviceRef_t device, char* serialNumber, uint16_t* size)
{
    fesd::BaseCommander* baseDevice;
    GetBaseCommander(device, baseDevice)

    FESD_C_CATCH_AND_RETURN
    (
        copyToCStr(serialNumber, baseDevice->getSerialNumber(), size);
    )
}

FESD_API int16_t FESD_GetFirmwareVersion(DeviceRef_t device, char* firmwareVersion, uint16_t* size)
{
    fesd::BaseCommander* baseDevice;
    GetBaseCommander(device, baseDevice)

    FESD_C_CATCH_AND_RETURN
    (
        copyToCStr(firmwareVersion, baseDevice->getFirmwareVersion(), size);
    )
}

FESD_API int16_t FESD_GetSystemRole(DeviceRef_t device, FESD_SystemRole_t* systemRole) {

    fesd::BaseCommander* baseDevice;
    GetBaseCommander(device, baseDevice)

    FESD_C_CATCH_AND_RETURN
    (
         *systemRole = static_cast<FESD_SystemRole_t>(baseDevice->getSystemRole());
    )
}


FESD_API int16_t FESD_SC2470ConfigureGain(DeviceRef_t device, FESD_Path_t path, double* gain)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *gain = sc2470Device->configureGain(static_cast<fesd::SC2470::Path>(path), *gain);
    )
}


FESD_API int16_t FESD_SC2470ConfigureAttenuation(DeviceRef_t device, FESD_Path_t path, double* attn)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *attn = sc2470Device->configureAttenuation(static_cast<fesd::SC2470::Path>(path), *attn);
    )
}

FESD_API int16_t FESD_SC2470ConfigureFrequenciesRfIf(DeviceRef_t device, FESD_Path_t path, double* rfHz, double* ifHz)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        fesd::SC2470::RfFrequency rfFreq = fesd::SC2470::RfFrequency(*rfHz);
        fesd::SC2470::IfFrequency ifFreq = fesd::SC2470::IfFrequency(*ifHz);
        fesd::SC2470::FrequencySet freqSet = sc2470Device->configureFrequencies(static_cast<fesd::SC2470::Path>(path), rfFreq, ifFreq);
        *rfHz = freqSet.rfHz;
        *ifHz = freqSet.ifHz;
    )
}
FESD_API int16_t FESD_SC2470ConfigureFrequenciesRfLo(DeviceRef_t device, FESD_Path_t path, double* rfHz, double* loHz)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        fesd::SC2470::RfFrequency rfFreq = fesd::SC2470::RfFrequency(*rfHz);
        fesd::SC2470::LoFrequency loFreq = fesd::SC2470::LoFrequency(*loHz);
        fesd::SC2470::FrequencySet freqSet = sc2470Device->configureFrequencies(static_cast<fesd::SC2470::Path>(path), rfFreq, loFreq);
        *rfHz = freqSet.rfHz;
        *loHz = freqSet.loHz;
    )
}
FESD_API int16_t FESD_SC2470ConfigureFrequenciesIfLo(DeviceRef_t device, FESD_Path_t path, double* ifHz, double* loHz)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        fesd::SC2470::IfFrequency ifFreq = fesd::SC2470::IfFrequency(*ifHz);
        fesd::SC2470::LoFrequency loFreq = fesd::SC2470::LoFrequency(*loHz);
        fesd::SC2470::FrequencySet freqSet = sc2470Device->configureFrequencies(static_cast<fesd::SC2470::Path>(path), ifFreq, loFreq);
        *ifHz = freqSet.ifHz;
        *loHz = freqSet.loHz;
    )
}
FESD_API int16_t FESD_SC2470ConfigureFrequencies(DeviceRef_t device, FESD_Path_t path, double* rfHz, double* ifHz, double* loHz)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        fesd::SC2470::FrequencySet freqSet;
        freqSet.rfHz = *rfHz;
        freqSet.ifHz = *ifHz;
        freqSet.loHz = *loHz;
        freqSet = sc2470Device->configureFrequencies(static_cast<fesd::SC2470::Path>(path), freqSet);
        *rfHz = freqSet.rfHz;
        *ifHz = freqSet.ifHz;
        *loHz = freqSet.loHz;
    )
}

FESD_API int16_t FESD_SC2470ConfigureBypassFrequency(DeviceRef_t device, FESD_Path_t path, double* byHz)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        fesd::SC2470::BypassFrequency byFreq = fesd::SC2470::BypassFrequency(*byHz);
        fesd::SC2470::FrequencySet freqSet = sc2470Device->configureBypassFrequency(static_cast<fesd::SC2470::Path>(path), byFreq);
        *byHz = freqSet.rfHz;
    )
}

FESD_API int16_t FESD_SC2470ConfigureLoOutput(DeviceRef_t device, FESD_Path_t path, bool* enable1x, bool* enable2x, uint16_t* powerLevel1x, uint16_t* powerLevel2x)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        fesd::SC2470::SynthesizerSettings settings = {*enable1x, *enable2x, *powerLevel1x, *powerLevel2x};
        settings = sc2470Device->configureSynthesizerSettings(static_cast<fesd::SC2470::Path>(path), settings);
        *enable1x = settings.enable1x;
        *enable2x = settings.enable2x;
        *powerLevel1x = settings.powerLevel1x;
        *powerLevel2x = settings.powerLevel2x;
    )
}

FESD_API int16_t FESD_SC2470ConfigureLoFrequency(DeviceRef_t device, FESD_Path_t path, double* freqHz)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *freqHz = sc2470Device->configureLoFrequency(static_cast<fesd::SC2470::Path>(path), *freqHz);
    )
}

FESD_API int16_t FESD_SC2470ConfigureDuplexSetting(DeviceRef_t device, FESD_DuplexSetting_t* setting)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *setting = static_cast<FESD_DuplexSetting_t>(sc2470Device->configureDuplexSetting(static_cast<fesd::SC2470::DuplexSetting>(*setting)));
    )
}


FESD_API int16_t FESD_SC2470ConfigurePhaseOffset(DeviceRef_t device, FESD_Path_t path, double* offset)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *offset = sc2470Device->configurePhaseOffset(static_cast<fesd::SC2470::Path>(path), *offset);
    )
}


FESD_API int16_t FESD_SC2470ConfigureReferenceSource(DeviceRef_t device, FESD_SC2470ReferenceSource_t* source)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *source = static_cast<FESD_SC2470ReferenceSource_t>(sc2470Device->configureReferenceSource(static_cast<fesd::SC2470::ReferenceSource>(*source)));
    )
}

FESD_API int16_t FESD_SC2470ConfigureInternalReferenceOverride(DeviceRef_t device, FESD_Path_t path, FESD_InternalReferenceFrequency_t* freqSel)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *freqSel = static_cast<FESD_InternalReferenceFrequency_t>(sc2470Device->configureInternalReferenceOverride(static_cast<fesd::SC2470::Path>(path), static_cast<fesd::SC2470::InternalReferenceFrequency>(*freqSel)));
    )
}


FESD_API int16_t FESD_SC2470ConfigureDCBias(DeviceRef_t device, FESD_Path_t path, int16_t* iBias,  int16_t* qBias)
{
    fesd::SC2470Commander* sc2470Device;
    fesd::SC2470::DCBias dcBias = fesd::SC2470::DCBias(*iBias, *qBias);
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        dcBias = sc2470Device->configureDCBias(static_cast<fesd::SC2470::Path>(path), dcBias);
        *iBias = dcBias.i;
        *qBias = dcBias.q;
    )
}

FESD_API int16_t FESD_SC2470ConfigureReferenceOutputEnable(DeviceRef_t device, bool* enable)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *enable = sc2470Device->configureReferenceOutputEnable(*enable);
    )
}

FESD_API int16_t FESD_SC2470ConfigureLoEnable(DeviceRef_t device, FESD_Path_t path, bool* enable)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *enable = sc2470Device->configureLoEnable(static_cast<fesd::SC2470::Path>(path), *enable);
    )
}

FESD_API int16_t FESD_SC2470GetGain(DeviceRef_t device, FESD_Path_t path, double* gainDb)
{
     fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *gainDb = sc2470Device->getGain(static_cast<fesd::SC2470::Path>(path));
    )
}

FESD_API int16_t FESD_SC2470GetGainLimits(DeviceRef_t device, FESD_Path_t path, double* minGainDb, double* maxGainDb)
{
     fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        fesd::SC2470::GainLimitsSet set = sc2470Device->getGainLimits(static_cast<fesd::SC2470::Path>(path));
        *maxGainDb = set.maxDb;
        *minGainDb = set.minDb;
    )
}

FESD_API int16_t FESD_SC2470GetAttenuation(DeviceRef_t device, FESD_Path_t path, double* attn)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *attn = sc2470Device->getAttenuation(static_cast<fesd::SC2470::Path>(path));
    )
}

FESD_API int16_t FESD_SC2470GetFrequencies(DeviceRef_t device, FESD_Path_t path, double* rfHz, double* ifHz, double* loHz)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        fesd::SC2470::FrequencySet freqSet = sc2470Device->getFrequencies(static_cast<fesd::SC2470::Path>(path));
        *rfHz = freqSet.rfHz;
        *ifHz = freqSet.ifHz;
        *loHz = freqSet.loHz;
    )
}


FESD_API int16_t FESD_SC2470GetLoOutput(DeviceRef_t device, FESD_Path_t path, bool* enable1x, bool* enable2x, uint16_t* powerLevel1x, uint16_t* powerLevel2x)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        fesd::SC2470::SynthesizerSettings settings = sc2470Device->getSynthesizerSettings(static_cast<fesd::SC2470::Path>(path));
        *enable1x = settings.enable1x;
        *enable2x = settings.enable2x;
        *powerLevel1x = settings.powerLevel1x;
        *powerLevel2x = settings.powerLevel2x;
    )
}

FESD_API int16_t FESD_SC2470GetLoFrequency(DeviceRef_t device, FESD_Path_t path, double* freq)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *freq = sc2470Device->getLoFrequency(static_cast<fesd::SC2470::Path>(path));
    )
}

FESD_API int16_t FESD_SC2470GetDuplexSetting(DeviceRef_t device, FESD_DuplexSetting_t* setting)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *setting = static_cast<FESD_DuplexSetting_t>(sc2470Device->getDuplexSetting());
    )
}


FESD_API int16_t FESD_SC2470GetPhaseOffset(DeviceRef_t device, FESD_Path_t path, double* offset)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *offset = sc2470Device->getPhaseOffset(static_cast<fesd::SC2470::Path>(path));
    )
}

FESD_API int16_t FESD_SC2470GetReferenceSource(DeviceRef_t device, FESD_SC2470ReferenceSource_t* source)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *source = static_cast<FESD_SC2470ReferenceSource_t>(sc2470Device->getReferenceSource());
    )
}

FESD_API int16_t FESD_SC2470GetSynthesizerMode(DeviceRef_t device, FESD_Path_t path, FESD_SC2470SynthesizerMode_t* mode)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)

    FESD_C_CATCH_AND_RETURN
    (
        *mode = static_cast<FESD_SC2470SynthesizerMode_t>(sc2470Device->getSynthesizerMode(static_cast<fesd::SC2470::Path>(path)));
    )
}

FESD_API int16_t FESD_SC2470GetDCBias(DeviceRef_t device, FESD_Path_t path, int16_t* iBias,  int16_t* qBias)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device);

    FESD_C_CATCH_AND_RETURN
    (
        fesd::SC2470::DCBias dcBias = sc2470Device->getDCBias(static_cast<fesd::SC2470::Path>(path));
        *iBias = dcBias.i;
        *qBias = dcBias.q;
    )
}

FESD_API int16_t FESD_SC2470GetReferenceOutputEnable(DeviceRef_t device, bool* enable)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)
    
    FESD_C_CATCH_AND_RETURN
    (
        *enable = sc2470Device->getReferenceOutputEnable();
    )
}

FESD_API int16_t FESD_SC2470GetLoEnable(DeviceRef_t device, FESD_Path_t path, bool* enable)
{
    fesd::SC2470Commander* sc2470Device;
    GetSC2470Commander(device, sc2470Device)
    
    FESD_C_CATCH_AND_RETURN
    (
        *enable = sc2470Device->getLoEnable(static_cast<fesd::SC2470::Path>(path));
    )
}


