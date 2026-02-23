#pragma once

#include <fesd/config.h>
#include <fesd/types/SC2470.hpp>
#include <fesd/BaseCommander.hpp>

#include <string>
#include <cstdint>
#include <map>

namespace fesd
{

class FESD_API SC2470Processor;
class DeviceConnection;

class FESD_API SC2470Commander final : public BaseCommander
{
public:
    SC2470Commander(std::shared_ptr<DeviceDetails> device);

    double configureGain(SC2470::Path path, double gainDb) const;
    double configureAttenuation(SC2470::Path path, double attenuationDb) const;
    SC2470::FrequencySet configureFrequencies(SC2470::Path path, SC2470::RfFrequency rfFreq, SC2470::IfFrequency ifFreq) const;
    SC2470::FrequencySet configureFrequencies(SC2470::Path path, SC2470::RfFrequency rfFreq, SC2470::LoFrequency loFreq) const;
    SC2470::FrequencySet configureFrequencies(SC2470::Path path, SC2470::IfFrequency ifFreq, SC2470::LoFrequency loFreq) const;
    SC2470::FrequencySet configureFrequencies(SC2470::Path path, SC2470::FrequencySet freqs) const;
    SC2470::FrequencySet configureBypassFrequency(SC2470::Path path, SC2470::BypassFrequency byFreq) const;
    SC2470::SynthesizerSettings configureSynthesizerSettings(SC2470::Path path, SC2470::SynthesizerSettings settings) const;
    bool configureLoEnable(SC2470::Path path, bool enable) const;
    double configureLoFrequency(SC2470::Path path, double frequencyHz) const; 
    SC2470::DuplexSetting configureDuplexSetting(SC2470::DuplexSetting setting) const;    
    SC2470::ReferenceSource configureReferenceSource(SC2470::ReferenceSource source) const;
    SC2470::InternalReferenceFrequency configureInternalReferenceOverride(SC2470::Path path, SC2470::InternalReferenceFrequency freq) const;
    double configurePhaseOffset(SC2470::Path path, double offset) const;
    SC2470::DCBias configureDCBias(SC2470::Path path, SC2470::DCBias bias) const;
    bool configureReferenceOutputEnable(bool enable) const;

    SC2470::GainLimitsSet getGainLimits(SC2470::Path path) const;
    double getGain(SC2470::Path path) const;
    double getAttenuation(SC2470::Path path) const; 
    SC2470::FrequencySet getFrequencies(SC2470::Path path) const;  
    SC2470::SynthesizerSettings getSynthesizerSettings(SC2470::Path path) const;
    bool getLoEnable(SC2470::Path path) const;
    double getLoFrequency(SC2470::Path path) const;
    SC2470::DuplexSetting getDuplexSetting(void) const;
    SC2470::ReferenceSource getReferenceSource(void) const;    
    SC2470::InternalReferenceFrequency getInternalReferenceOverride(SC2470::Path path) const;
    SC2470::SynthesizerMode getSynthesizerMode(SC2470::Path path) const;
    double getPhaseOffset(SC2470::Path path) const;  
    SC2470::DCBias getDCBias(SC2470::Path path) const;
    bool getReferenceOutputEnable(void) const;
    
private:
#pragma warning(push) 
#pragma warning(disable:4251)
    std::shared_ptr<SC2470Processor> m_coProcessor;
#pragma warning(pop) 
    bool useAttn = false;
};

} // namespace fesd