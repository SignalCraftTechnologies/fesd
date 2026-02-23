#pragma once

#include "types/DeviceDetails.hpp"

#include <fesd/types/Common.hpp>
#include <fesd/types/SC2470.hpp>

#include <cstdint>

namespace fesd
{

class SC2470Processor final
{
public:
    enum class ClockSource
    {
        External, 
        Internal,
    };
    
    enum class SynthsizerReferenceFreq
    {
        Freq100MHz,
        Freq105MHz,
    };

    enum class ReferenceFreq
    {
        Freq10MHz,
        Freq100MHz,
    };

    struct FrequencySet
    {
        double rfKHz;
        double ifKHz;
        double loKHz;
    };

    struct GainLimitsSet
    {
        double minDb;
        double maxDb;
    };

    enum class DuplexSetting
    {
        FDD,
        TDD
    };

    struct AttenuatorRxSet
    {
        double attnADb;
        double attnBDb;
    };

    struct SynthesizerPowerSet
    {
        uint16_t power1x;
        uint16_t power2x;
    };

    struct SynthesizerEnableSet
    {
        bool enable1x;
        bool enable2x;
    };

    struct ReferenceConfig 
    {
        ClockSource clkSource;
        ReferenceFreq freqSel;
    };

    struct SynthesizerRfRegisters
    {
        uint16_t intDivider;
        uint32_t frac1;
        uint32_t frac2;
        uint32_t mod2;
        uint16_t rfDivider;
    };

public:
    SC2470Processor(std::shared_ptr<DeviceDetails> details) : m_details(details) {};
public:
    double getAttnTx(void) const;
    void setAttnTx(double attnDb) const;
    AttenuatorRxSet getAttnRx(void) const;
    void setAttnRx(AttenuatorRxSet attns) const;
    SC2470Processor::GainLimitsSet getGainLimits(SC2470::Path path) const;
    double getGain(SC2470::Path path) const;
    void setGain(SC2470::Path path, double gainDb) const;
    SC2470Processor::DuplexSetting getRfPath(void) const;
    void setRfPath(SC2470Processor::DuplexSetting duplex) const;
    SC2470::Path getTddPath(void) const;
    void setTddPath(SC2470::Path setting) const;
    SC2470Processor::SynthesizerPowerSet getSynthPowerLevel(SC2470::Path path) const;
    void setSynthPowerLevel(SC2470::Path path, SC2470Processor::SynthesizerPowerSet powerSet) const;
    SC2470Processor::SynthsizerReferenceFreq getSynthReferenceFrequency(SC2470::Path path) const;
    void setSynthReferenceFrequency(SC2470::Path path, SC2470Processor::SynthsizerReferenceFreq freq) const;
    SC2470Processor::FrequencySet getFrequencies(SC2470::Path path) const;
    bool getSynthReferenceAuto(SC2470::Path path) const;
    void setSynthReferenceAuto(SC2470::Path path, bool enable) const;
    void setFrequencies(SC2470::Path path, SC2470Processor::FrequencySet freqs) const;
    void setBypassFrequency(SC2470::Path path, double freq) const;
    double getLoFrequencyKHz(SC2470::Path path) const;
    void setLoFrequencyKHz(SC2470::Path path, double frequencyKhz) const;
    double getOutputPower(SC2470::Path path) const;
    void setOutputPower(SC2470::Path path, double powerDb) const;
    double getPaBiasCurrent(uint16_t paId) const;
    void setPaBiasCurrent(uint16_t paId, double currentSetpoint) const;
    double getPaBiasVoltage(uint16_t paId) const;
    double getPaDrainVoltage(uint16_t paId) const;
    double getPaTemp() const;
    double getReferenceDac() const;
    void setReferenceDac(uint32_t dacValue) const;
    SC2470Processor::ReferenceConfig getReferenceConfig() const;
    void setReferenceConfig(const SC2470Processor::ReferenceConfig& config) const;
    double getReferenceLockDetect() const;
    bool getReferenceOutputEnable() const;
    void setReferenceOutputEnable(bool enable) const;
    bool getForceFractionalMode(SC2470::Path path) const;
    void setForceFractionalMode(SC2470::Path path, bool enable) const;
    void incrementPhase(SC2470::Path path, double increment) const;
    double getPhaseAccumulator(SC2470::Path path) const;
    void setPhaseAccumulator(SC2470::Path path, double phase) const;
    SynthesizerEnableSet getSynthEnable(SC2470::Path path) const;
    void setSynthEnable(SC2470::Path path, SynthesizerEnableSet enables) const;
    SC2470Processor::SynthesizerRfRegisters getSynthRfSet(SC2470::Path path) const;
    void configApplyToSystem(void) const;
    void configLoadFromDefault(void) const;
    void configLoadFromNvm(void) const;
    void configSavetoNvm(void) const;
    bool getConfigAutoload(void) const;
    void setConfigAutoload(bool enable) const;
    bool getConfigAutoPhase(void) const;
    void setConfigAutoPhase(bool enable) const;
    bool getLoClkEnable(SC2470::Path path) const;
    void setLoClkEnable(SC2470::Path path, bool enable) const;
    void setDCBias(SC2470::Path path, SC2470::DCBias bias) const;
    SC2470::DCBias getDCBias(SC2470::Path path) const;

private:
#pragma warning(push) 
#pragma warning(disable:4251)
    std::shared_ptr<DeviceDetails> m_details;
#pragma warning(pop)   
};

} // namespace fesd