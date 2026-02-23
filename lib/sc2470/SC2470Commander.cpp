#include <fesd/SC2470Commander.hpp>
#include <fesd/types/Exception.hpp>
#include "SC2470Processor.hpp"
#include "GeneralProcessor.hpp"
#include "DeviceConnection.hpp"
#include "Utility.hpp"

#include <cmath>

namespace 
{

} // static namespace

namespace fesd
{

SC2470Commander::SC2470Commander(std::shared_ptr<DeviceDetails> device)
    : BaseCommander(device),
    m_coProcessor(std::make_shared<SC2470Processor>(device))
{
    // Will throw an exception if invalid parameters are given
    m_genProcessor->getId();
};

SC2470::GainLimitsSet SC2470Commander::getGainLimits(SC2470::Path path) const
{
    SC2470Processor::GainLimitsSet procGainLimits = m_coProcessor->getGainLimits(path);
    if (utility::isAlmostEqualToZero(procGainLimits.maxDb - procGainLimits.minDb, 0.001)) {
        throw CalibrationError("Gain limits have zero range");
    }
    return {procGainLimits.minDb, procGainLimits.maxDb};
}

double SC2470Commander::getGain(SC2470::Path path) const
{
    return m_coProcessor->getGain(path);
}

double SC2470Commander::configureGain(SC2470::Path path, double gainDb) const
{
    SC2470::GainLimitsSet gainLimits = getGainLimits(path);
    if(gainDb > gainLimits.maxDb) {
        gainDb = gainLimits.maxDb;
    } else if (gainDb < gainLimits.minDb) {
        gainDb = gainLimits.minDb;
    }
    m_coProcessor->setGain(path, gainDb);
    return this->getGain(path);
}

double SC2470Commander::getAttenuation(SC2470::Path path) const
{    
    if (path == SC2470::Path::TX)
        return m_coProcessor->getAttnTx();
    else
    {
        SC2470Processor::AttenuatorRxSet rxValues = m_coProcessor->getAttnRx();
        return rxValues.attnADb + rxValues.attnBDb;
    }
}

double SC2470Commander::configureAttenuation(SC2470::Path path, double attenuationDb) const
{
    if (path == SC2470::Path::TX)
    {
        if (attenuationDb < 0)
            attenuationDb = 0;
        if (attenuationDb > 31.5)
            attenuationDb = 31.5;

        m_coProcessor->setAttnTx(attenuationDb);
        return this->getAttenuation(path);
    }
    else
    {
        if (attenuationDb < 0)
            attenuationDb = 0;
        if (attenuationDb > 63.25)
            attenuationDb = 63.25;

        // Apply roughly equally accross attns
        // AttnA will lead in upticks
        SC2470Processor::AttenuatorRxSet rxValues;

        rxValues.attnBDb = 0.5 * round(attenuationDb);
        rxValues.attnADb = attenuationDb - rxValues.attnBDb;

        m_coProcessor->setAttnRx(rxValues);
        return this->getAttenuation(path);
    }
}

SC2470::FrequencySet SC2470Commander::getFrequencies(SC2470::Path path) const
{
    SC2470::FrequencySet freqsHz;
    SC2470Processor::FrequencySet freqsKHz;

    freqsKHz = m_coProcessor->getFrequencies(path);

    freqsHz.rfHz = freqsKHz.rfKHz * 1000;
    freqsHz.ifHz = freqsKHz.ifKHz * 1000;
    freqsHz.loHz = freqsKHz.loKHz * 1000;

    return freqsHz;
}

SC2470::FrequencySet SC2470Commander::configureFrequencies(SC2470::Path path, SC2470::RfFrequency rfFreq, SC2470::IfFrequency ifFreq) const
{
    SC2470Processor::FrequencySet freqsKHz;

    freqsKHz.loKHz = 0.0;
    if ((rfFreq.rfHz - ifFreq.ifHz) < SC2470::LoFrequency::loMinHz) {
        freqsKHz.rfKHz = (ifFreq.ifHz + SC2470::LoFrequency::loMinHz) / 1000.0;
    } else {
        freqsKHz.rfKHz = rfFreq.rfHz / 1000.0;
    }
    freqsKHz.ifKHz = ifFreq.ifHz / 1000.0;
    m_coProcessor->setFrequencies(path, freqsKHz);
    return this->getFrequencies(path);
}

SC2470::FrequencySet SC2470Commander::configureFrequencies(SC2470::Path path, SC2470::RfFrequency rfFreq, SC2470::LoFrequency loFreq) const
{
    SC2470::FrequencySet freqsHz;
    SC2470Processor::FrequencySet freqsKHz;

    freqsKHz.ifKHz = 0.0;

    if ((rfFreq.rfHz - loFreq.loHz) < SC2470::IfFrequency::ifMinHz)
    {
        if ((rfFreq.rfHz - SC2470::LoFrequency::loMinHz) > SC2470::IfFrequency::ifMinHz)
        {
            freqsKHz.rfKHz = (rfFreq.rfHz / 1000.0);
            freqsKHz.loKHz = (SC2470::LoFrequency::loMinHz / 1000.0);
        }
        else 
        {
            freqsKHz.rfKHz = ((SC2470::LoFrequency::loMinHz + SC2470::IfFrequency::ifMinHz) / 1000.0);
            freqsKHz.loKHz = (SC2470::LoFrequency::loMinHz / 1000.0);
        }
    }
    else if ((rfFreq.rfHz - loFreq.loHz) > SC2470::IfFrequency::ifMaxHz) {
        freqsKHz.rfKHz = (loFreq.loHz + SC2470::IfFrequency::ifMaxHz) / 1000.0;
        freqsKHz.loKHz = loFreq.loHz / 1000.0;
    }
    else
    {
        freqsKHz.rfKHz = rfFreq.rfHz / 1000.0;
        freqsKHz.loKHz = loFreq.loHz / 1000.0;
    }

    m_coProcessor->setFrequencies(path, freqsKHz);
    return this->getFrequencies(path);
}

SC2470::FrequencySet SC2470Commander::configureFrequencies(SC2470::Path path, SC2470::IfFrequency ifFreq, SC2470::LoFrequency loFreq) const
{
    SC2470::FrequencySet freqsHz;
    SC2470Processor::FrequencySet freqsKHz;

    freqsKHz.rfKHz = 0.0;

    if ((loFreq.loHz + ifFreq.ifHz) < SC2470::RfFrequency::rfMinHz)
    {
        freqsKHz.loKHz = (ifFreq.ifHz + SC2470::RfFrequency::rfMinHz) / 1000.0;
    }
    else if (((loFreq.loHz + ifFreq.ifHz) > SC2470::RfFrequency::rfMaxHz))
    {
        freqsKHz.loKHz = (SC2470::RfFrequency::rfMaxHz - ifFreq.ifHz) / 1000.0;
    } 
    else
    {
        freqsKHz.loKHz = loFreq.loHz / 1000.0;
    }

    freqsKHz.ifKHz = ifFreq.ifHz / 1000.0;

    m_coProcessor->setFrequencies(path, freqsKHz);
    return this->getFrequencies(path);
}

SC2470::FrequencySet SC2470Commander::configureFrequencies(SC2470::Path path, SC2470::FrequencySet freqs) const
{
    SC2470::FrequencySet coercedFreqs;
    SC2470Processor::FrequencySet freqsKHz;

    coercedFreqs.rfHz = utility::isAlmostEqualToZero(freqs.rfHz, 0.001) ? 0.0 : SC2470::RfFrequency(freqs.rfHz).rfHz;
    coercedFreqs.ifHz = utility::isAlmostEqualToZero(freqs.ifHz, 0.001) ? 0.0 : SC2470::IfFrequency(freqs.ifHz).ifHz;
    coercedFreqs.loHz = utility::isAlmostEqualToZero(freqs.loHz, 0.001) ? 0.0 : SC2470::LoFrequency(freqs.loHz).loHz;

    freqsKHz.rfKHz = coercedFreqs.rfHz / 1000.0;
    freqsKHz.ifKHz = coercedFreqs.ifHz / 1000.0;
    freqsKHz.loKHz = coercedFreqs.loHz / 1000.0;

    m_coProcessor->setFrequencies(path, freqsKHz);
    return this->getFrequencies(path);
}

SC2470::FrequencySet SC2470Commander::configureBypassFrequency(SC2470::Path path, SC2470::BypassFrequency byFreq) const
{
    SC2470::FrequencySet freqsHz;
    SC2470Processor::FrequencySet freqsKHz;

    freqsKHz.loKHz = 0.0;

    freqsKHz.rfKHz = byFreq.byHz / 1000.0;
    freqsKHz.ifKHz = byFreq.byHz / 1000.0;

    m_coProcessor->setFrequencies(path, freqsKHz);
    return this->getFrequencies(path);

}


SC2470::SynthesizerSettings SC2470Commander::getSynthesizerSettings(SC2470::Path path) const
{
    SC2470Processor::SynthesizerPowerSet power = m_coProcessor->getSynthPowerLevel(path);
    SC2470Processor::SynthesizerEnableSet enable = m_coProcessor->getSynthEnable(path);
    return {enable.enable1x, enable.enable2x, power.power1x, power.power2x};
}

SC2470::SynthesizerSettings SC2470Commander::configureSynthesizerSettings(SC2470::Path path, SC2470::SynthesizerSettings settings) const
{
    m_coProcessor->setSynthPowerLevel(path, {settings.powerLevel1x, settings.powerLevel2x});
    m_coProcessor->setSynthEnable(path, {settings.enable1x, settings.enable2x});

    return this->getSynthesizerSettings(path);
} 

bool SC2470Commander::getLoEnable(SC2470::Path path) const
{
    return m_coProcessor->getLoClkEnable(path);
}

bool SC2470Commander::configureLoEnable(SC2470::Path path, bool enable) const
{
    m_coProcessor->setLoClkEnable(path, enable);
    return this->getLoEnable(path);
}

SC2470::DuplexSetting SC2470Commander::getDuplexSetting(void) const
{
    if (m_coProcessor->getRfPath() == SC2470Processor::DuplexSetting::FDD)
        return SC2470::DuplexSetting::Fdd;
    if (m_coProcessor->getTddPath() == SC2470::Path::TX)
        return SC2470::DuplexSetting::TddTx;
    return SC2470::DuplexSetting::TddRx;
}

SC2470::DuplexSetting SC2470Commander::configureDuplexSetting(SC2470::DuplexSetting setting) const
{
    switch (setting)
    {
        case SC2470::DuplexSetting::Fdd:
            m_coProcessor->setRfPath(SC2470Processor::DuplexSetting::FDD);
            break;
        case SC2470::DuplexSetting::TddRx:
            m_coProcessor->setRfPath(SC2470Processor::DuplexSetting::TDD);
            m_coProcessor->setTddPath(SC2470::Path::RX);
            break;
        case SC2470::DuplexSetting::TddTx:
            m_coProcessor->setRfPath(SC2470Processor::DuplexSetting::TDD);
            m_coProcessor->setTddPath(SC2470::Path::TX);
            break;
    }

    return this->getDuplexSetting();
}

SC2470::InternalReferenceFrequency SC2470Commander::getInternalReferenceOverride(SC2470::Path path) const
{
    if(m_coProcessor->getSynthReferenceAuto(path)) {
        return SC2470::InternalReferenceFrequency::Automatic;
    } else if (m_coProcessor->getSynthReferenceFrequency(path) == SC2470Processor::SynthsizerReferenceFreq::Freq100MHz)
        return SC2470::InternalReferenceFrequency::Force100MHz;
    return SC2470::InternalReferenceFrequency::Force105MHz;
}

SC2470::InternalReferenceFrequency SC2470Commander::configureInternalReferenceOverride(SC2470::Path path, SC2470::InternalReferenceFrequency freq) const
{
    switch(freq) {
    case SC2470::InternalReferenceFrequency::Automatic:
        m_coProcessor->setSynthReferenceAuto(path, true);
        break;
    case SC2470::InternalReferenceFrequency::Force100MHz:
        m_coProcessor->setSynthReferenceAuto(path, false);
        m_coProcessor->setSynthReferenceFrequency(path, SC2470Processor::SynthsizerReferenceFreq::Freq100MHz);
        break;
    case SC2470::InternalReferenceFrequency::Force105MHz:
        m_coProcessor->setSynthReferenceAuto(path, false);
        m_coProcessor->setSynthReferenceFrequency(path, SC2470Processor::SynthsizerReferenceFreq::Freq105MHz);
        break;
    }

    return this->getInternalReferenceOverride(path);
}


double SC2470Commander::getLoFrequency(SC2470::Path path) const
{
    return m_coProcessor->getLoFrequencyKHz(path) * 1000.0;
}

double SC2470Commander::configureLoFrequency(SC2470::Path path, double frequencyHz) const
{
    if (frequencyHz < SC2470::LoFrequency::loMinHz)
        frequencyHz = SC2470::LoFrequency::loMinHz;
    if (frequencyHz > SC2470::LoFrequency::loMaxHz)
        frequencyHz = SC2470::LoFrequency::loMaxHz;

    m_coProcessor->setLoFrequencyKHz(path, (frequencyHz / 1000.0));

    return this->getLoFrequency(path);
}

SC2470::ReferenceSource SC2470Commander::configureReferenceSource(SC2470::ReferenceSource source) const 
{
    SC2470Processor::ReferenceConfig config;

    double freqRx = m_coProcessor->getLoFrequencyKHz(SC2470::Path::RX);
    double freqTx = m_coProcessor->getLoFrequencyKHz(SC2470::Path::TX);

    switch (source)
    {
        case SC2470::ReferenceSource::Internal:
            config.clkSource = SC2470Processor::ClockSource::Internal;
            m_coProcessor->setReferenceConfig(config);
            break;
        case SC2470::ReferenceSource::External10MHz:
            config.clkSource = SC2470Processor::ClockSource::External;
            config.freqSel = SC2470Processor::ReferenceFreq::Freq10MHz;
            m_coProcessor->setReferenceConfig(config);            
            break;
        case SC2470::ReferenceSource::External100MHz:
            config.clkSource = SC2470Processor::ClockSource::External;
            config.freqSel = SC2470Processor::ReferenceFreq::Freq100MHz;
            m_coProcessor->setReferenceConfig(config);            
            break;
        default:
            break; 
    }

    // Reconfigure frequencies after changing source
    this->configureLoFrequency(SC2470::Path::RX, freqRx);
    this->configureLoFrequency(SC2470::Path::TX, freqTx);
    
    return this->getReferenceSource();
}

SC2470::ReferenceSource SC2470Commander::getReferenceSource(void) const
{
    SC2470Processor::ReferenceConfig config = m_coProcessor->getReferenceConfig();

    if (config.clkSource == SC2470Processor::ClockSource::Internal)
        return SC2470::ReferenceSource::Internal;
    else if (config.clkSource== SC2470Processor::ClockSource::External)
    {
        if (config.freqSel == SC2470Processor::ReferenceFreq::Freq10MHz)
            return SC2470::ReferenceSource::External10MHz;
        else if (config.freqSel == SC2470Processor::ReferenceFreq::Freq100MHz)
            return SC2470::ReferenceSource::External100MHz;
    }

    throw CommunicationError("Invalid response from device...");
}

SC2470::SynthesizerMode SC2470Commander::getSynthesizerMode(SC2470::Path path) const
{
    SC2470Processor::SynthesizerRfRegisters registers = m_coProcessor->getSynthRfSet(path);

    bool forced = m_coProcessor->getForceFractionalMode(path);
    // If Frac1, Frac2, and Mod2 are all 0, we are in integer mode
    if (!forced && (registers.frac1 == 0 && registers.frac2 == 0))
        return SC2470::SynthesizerMode::Integer;
    return SC2470::SynthesizerMode::Fractional;
}


double SC2470Commander::configurePhaseOffset(SC2470::Path path, double offset) const 
{
    double freqHz = this->getLoFrequency(path);

    if (offset == 0)
    {
        m_coProcessor->setPhaseAccumulator(path, 0);
        m_coProcessor->setForceFractionalMode(path, false);
        // We must reissue freq command for the synth to switch out of fractional mode
        this->configureLoFrequency(path, freqHz);  // this resets phase to 0
        return this->getPhaseOffset(path);
    }

    if (!m_coProcessor->getForceFractionalMode(path))
    {
        m_coProcessor->setForceFractionalMode(path, true);
        // We must reissue freq command for the synth to switch into fractional mode
        this->configureLoFrequency(path, freqHz);  // this resets phase to 0
    }

    if (offset < -360)
        offset = -360;
    else if (offset > 360)
        offset = 360;

    double currentPhase = m_coProcessor->getPhaseAccumulator(path);
    double increment = offset - currentPhase;
    m_coProcessor->incrementPhase(path, increment);

    return m_coProcessor->getPhaseAccumulator(path); 
}

double SC2470Commander::getPhaseOffset(SC2470::Path path) const
{
    if (!m_coProcessor->getForceFractionalMode(path))
        return 0;
    return m_coProcessor->getPhaseAccumulator(path);
}

SC2470::DCBias SC2470Commander::configureDCBias(SC2470::Path path, SC2470::DCBias bias) const
{
    m_coProcessor->setDCBias(path, bias);
    return this->getDCBias(path);
}   

SC2470::DCBias SC2470Commander::getDCBias(SC2470::Path path) const
{
    return m_coProcessor->getDCBias(path);
}

bool SC2470Commander::configureReferenceOutputEnable(bool enable) const
{
    m_coProcessor->setReferenceOutputEnable(enable);
    return this->getReferenceOutputEnable();
}

bool SC2470Commander::getReferenceOutputEnable(void) const
{
    return m_coProcessor->getReferenceOutputEnable();
}

} // namespace fesd