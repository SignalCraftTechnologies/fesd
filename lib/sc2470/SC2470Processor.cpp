#include "SC2470Processor.hpp"
#include "DeviceConnection.hpp"
#include "MessageBuilder.hpp"
#include "Utility.hpp"
#include <fesd/types/Exception.hpp>

#include <boost/algorithm/string.hpp>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
// Commands
// const std::string paBiasCurrent = "BIAS:CUR";
// const std::string paBiasVolt = "BIAS:BVOLT";
const std::string paDrainVolt = "BIAS:PAVOLT";
const std::string paBiasTemp = "BIAS:TEMP";

const std::string configApply = "CONFIG:APPLY";
const std::string configDefault = "CONFIG:DEFAULT";
const std::string configLoad = "CONFIG:LOAD";
const std::string configSave = "CONFIG:SAVE";
const std::string configAutoLoad = "CONFIG:AUTOLOAD";
const std::string configAutoPhase = "CONFIG:AUTOPHASE";

const std::string ifPathAttn = "IFPATH:ATTN";
const std::string ifPathDCBias = "IFPATH:DCBIAS";

const std::string loClkFreq = "LOCLK:FREQ";
const std::string loClkEn = "LOCLK:EN";
const std::string phaseIncrement = "LOCLK:PHINC";
const std::string phaseAccumulator = "LOCLK:PHCUMU";

const std::string pathFreq = "PATH:FREQ";
const std::string pathGain = "PATH:GAIN";
const std::string pathGainLim = "PATH:GAINLIM";

const std::string refDac = "REFDAC:DAC";
const std::string refConfig = "REFPLL:CONFIG";
const std::string refLockDetect = "REFPLL:LD";
const std::string refOutputEnable = "REFPLL:OUTPUT";

const std::string rfPathPath = "RFPATH:PATH";
const std::string rfPathTdd = "RFPATH:TDD";
const std::string rfPathAttn = "RFPATH:ATTN";

const std::string synthPower = "SYN:POW";
const std::string synthRefFreq = "SYN:REFFREQ";
const std::string synthRefAuto = "SYN:AUTOREF";
const std::string synthRefSource = "SYN:REFSRC";
const std::string synthFractionalMode = "SYN:FFRAC";
const std::string synthEnable = "SYN:EN";
const std::string synthRfSet = "SYN:RFSET";

const std::map<fesd::SC2470::Path, std::string> PathStringMap = {
   {fesd::SC2470::Path::RX, "RX"},
   {fesd::SC2470::Path::TX, "TX"},
};

const std::map<fesd::SC2470Processor::ClockSource, std::string> ClockSourceStringMap = {
   {fesd::SC2470Processor::ClockSource::External, "EXT"},
   {fesd::SC2470Processor::ClockSource::Internal, "INT"},
};

const std::map<bool, std::string> EnableStringMap = {
   {false, "OFF"},
   {true,  "ON" },
};

const std::map<fesd::SC2470Processor::ReferenceFreq, double> ReferenceFreqKhzMap = {
   {fesd::SC2470Processor::ReferenceFreq::Freq10MHz,  10000.0 },
   {fesd::SC2470Processor::ReferenceFreq::Freq100MHz, 100000.0},
};

const std::map<fesd::SC2470Processor::SynthsizerReferenceFreq, double> SynthReferenceFreqKhzMap = {
   {fesd::SC2470Processor::SynthsizerReferenceFreq::Freq100MHz, 100000.0},
   {fesd::SC2470Processor::SynthsizerReferenceFreq::Freq105MHz, 105000.0},
};

const std::map<fesd::SC2470Processor::DuplexSetting, std::string> DuplexStringMap = {
   {fesd::SC2470Processor::DuplexSetting::FDD, "FDD"},
   {fesd::SC2470Processor::DuplexSetting::TDD, "TDD"},
};

inline void throwCommsError(void)
{
    throw fesd::CommunicationError("Invalid response from device...");
}

} // namespace

namespace fesd {

double SC2470Processor::getAttnTx(void) const
{
    return std::stod(m_details->connection->transact(MessageBuilder::buildQuery(rfPathAttn, m_details->slotId, SC2470::Path::TX)));
}

void SC2470Processor::setAttnTx(double attnDb) const
{
    const std::vector<std::string> params{std::to_string(attnDb)};
    m_details->connection->transact(MessageBuilder::buildCommand(rfPathAttn, m_details->slotId, SC2470::Path::TX, params));
}

SC2470Processor::GainLimitsSet SC2470Processor::getGainLimits(SC2470::Path path) const
{
    const std::string result = m_details->connection->transact(MessageBuilder::buildQuery(pathGainLim, m_details->slotId, path));
    std::vector<std::string> splitResult;

    boost::split(splitResult, result, boost::is_any_of(" "));

    return {std::stod(splitResult[0]), std::stod(splitResult[1])};
}

double SC2470Processor::getGain(SC2470::Path path) const
{
    return std::stod(m_details->connection->transact(MessageBuilder::buildQuery(pathGain, m_details->slotId, path)));
}

void SC2470Processor::setGain(SC2470::Path path, double gainDb) const
{
    std::vector<std::string> params{std::to_string(gainDb)};
    m_details->connection->transact(MessageBuilder::buildCommand(pathGain, m_details->slotId, path, params));
}


SC2470Processor::AttenuatorRxSet SC2470Processor::getAttnRx(void) const
{
    const std::string result = m_details->connection->transact(MessageBuilder::buildQuery(ifPathAttn, m_details->slotId, SC2470::Path::RX));
    std::vector<std::string> splitResult;

    boost::split(splitResult, result, boost::is_any_of(" "));

    return {std::stod(splitResult[0]), std::stod(splitResult[1])};
}

void SC2470Processor::setAttnRx(SC2470Processor::AttenuatorRxSet attns) const
{
    const std::vector<std::string> params{std::to_string(attns.attnADb), std::to_string(attns.attnBDb)};
    m_details->connection->transact(MessageBuilder::buildCommand(ifPathAttn, m_details->slotId, SC2470::Path::RX, params));
}

SC2470Processor::DuplexSetting SC2470Processor::getRfPath(void) const
{
    const std::string result = m_details->connection->transact(MessageBuilder::buildQuery(rfPathPath, m_details->slotId));

    for (const auto& [key, value] : DuplexStringMap)
    {
        if (result.substr(0, 3).compare(value) == 0) return key;
    }

    throwCommsError();
    // Never gets here, this is to fix warning
    return SC2470Processor::DuplexSetting::FDD;
}

void SC2470Processor::setRfPath(SC2470Processor::DuplexSetting duplex) const
{
    const std::vector<std::string> params{DuplexStringMap.at(duplex)};
    m_details->connection->transact(MessageBuilder::buildCommand(rfPathPath, m_details->slotId, params));
}

SC2470::Path SC2470Processor::getTddPath(void) const
{
    const std::string result = m_details->connection->transact(MessageBuilder::buildQuery(rfPathTdd, m_details->slotId));

    for (const auto& [key, value] : PathStringMap)
    {
        if (result.compare(value) == 0) return key;
    }

    throwCommsError();
    // Never gets here, this is to fix warning
    return SC2470::Path::RX;
}

void SC2470Processor::setTddPath(SC2470::Path setting) const
{
    m_details->connection->transact(MessageBuilder::buildCommand(rfPathTdd, m_details->slotId, setting));
}

SC2470Processor::SynthesizerPowerSet SC2470Processor::getSynthPowerLevel(SC2470::Path path) const
{
    SC2470Processor::SynthesizerPowerSet resultSet;
    std::vector<std::string> splitResult;

    const std::string result = m_details->connection->transact(MessageBuilder::buildQuery(synthPower, m_details->slotId, path));

    boost::split(splitResult, result, boost::is_any_of(" "));

    resultSet.power1x = std::stoul(splitResult[0]);
    resultSet.power2x = std::stoul(splitResult[1]);

    return resultSet;
}

void SC2470Processor::setSynthPowerLevel(SC2470::Path path, SC2470Processor::SynthesizerPowerSet powerSet) const
{
    std::vector<std::string> params{std::to_string(powerSet.power1x), std::to_string(powerSet.power2x)};
    m_details->connection->transact(MessageBuilder::buildCommand(synthPower, m_details->slotId, path, params));
}

SC2470Processor::SynthsizerReferenceFreq SC2470Processor::getSynthReferenceFrequency(SC2470::Path path) const
{
    const double result = std::stod(m_details->connection->transact(MessageBuilder::buildQuery(synthRefFreq, m_details->slotId, path)));

    for (const auto& [key, value] : SynthReferenceFreqKhzMap)
    {
        if (utility::isAlmostEqual(result, value)) return key;
    }

    throwCommsError();
    // Never gets here, this is to fix warning
    return SC2470Processor::SynthsizerReferenceFreq::Freq100MHz;
}

void SC2470Processor::setSynthReferenceFrequency(SC2470::Path path, SC2470Processor::SynthsizerReferenceFreq freq) const
{
    const std::vector<std::string> params({std::to_string(SynthReferenceFreqKhzMap.at(freq))});
    m_details->connection->transact(MessageBuilder::buildCommand(synthRefFreq, m_details->slotId, path, params));
}

bool SC2470Processor::getSynthReferenceAuto(SC2470::Path path) const {
    std::string result = m_details->connection->transact(MessageBuilder::buildQuery(synthRefAuto, m_details->slotId, path));

    if (result == "0") return false;
    else if (result == "1") return true;
    throwCommsError();
    // Never gets here, this is to fix warning
    return false;
}

void SC2470Processor::setSynthReferenceAuto(SC2470::Path path, bool enable) const {
    std::vector<std::string> params{enable ? "1" : "0"};
    m_details->connection->transact(MessageBuilder::buildCommand(synthRefAuto, m_details->slotId, path, params));
}

SC2470Processor::FrequencySet SC2470Processor::getFrequencies(SC2470::Path path) const
{
    SC2470Processor::FrequencySet resultSet;
    std::vector<std::string> splitResult;

    const std::string result = m_details->connection->transact(MessageBuilder::buildQuery(pathFreq, m_details->slotId, path));

    boost::split(splitResult, result, boost::is_any_of(" "));

    resultSet.rfKHz = std::stod(splitResult[0]);
    resultSet.ifKHz = std::stod(splitResult[1]);
    resultSet.loKHz = std::stod(splitResult[2]);

    return resultSet;
}

void SC2470Processor::setFrequencies(SC2470::Path path, SC2470Processor::FrequencySet freqs) const
{
    const std::vector<std::string> params{std::to_string(freqs.rfKHz), std::to_string(freqs.ifKHz), std::to_string(freqs.loKHz)};
    m_details->connection->transact(MessageBuilder::buildCommand(pathFreq, m_details->slotId, path, params));
}

void SC2470Processor::setBypassFrequency(SC2470::Path path, double freq) const
{
    const std::vector<std::string> params{std::to_string(freq), std::to_string(freq), std::to_string(0.0)};
    m_details->connection->transact(MessageBuilder::buildCommand(pathFreq, m_details->slotId, path, params));
}

double SC2470Processor::getLoFrequencyKHz(SC2470::Path path) const
{
    return std::stod(m_details->connection->transact(MessageBuilder::buildQuery(loClkFreq, m_details->slotId, path)));
}

void SC2470Processor::setLoFrequencyKHz(SC2470::Path path, double frequencyKhz) const
{
    std::vector<std::string> params{std::to_string(frequencyKhz)};
    m_details->connection->transact(MessageBuilder::buildCommand(loClkFreq, m_details->slotId, path, params));
}
/*
double SC2470Processor::getPaBiasCurrent(uint16_t paId) const
{
    std::vector<std::string> params{std::to_string(paId)};
    return std::stod(m_details->connection->transact(MessageBuilder::buildQuery(paBiasCurrent, m_details->slotId, params)));
}

void SC2470Processor::setPaBiasCurrent(uint16_t paId, double currentSetpoint) const
{
    std::vector<std::string> params{std::to_string(paId), std::to_string(currentSetpoint)};
    m_details->connection->transact(MessageBuilder::buildCommand(paBiasCurrent, m_details->slotId, params));
}

double SC2470Processor::getPaBiasVoltage(uint16_t paId) const
{
    std::vector<std::string> params{std::to_string(paId)};
    return std::stod(m_details->connection->transact(MessageBuilder::buildQuery(paBiasVolt, m_details->slotId, params)));
}
*/
double SC2470Processor::getPaDrainVoltage(uint16_t paId) const
{
    std::vector<std::string> params{std::to_string(paId)};
    return std::stod(m_details->connection->transact(MessageBuilder::buildQuery(paDrainVolt, m_details->slotId, params)));
}

double SC2470Processor::getPaTemp() const
{
    return std::stod(m_details->connection->transact(MessageBuilder::buildQuery(paBiasTemp, m_details->slotId)));
}

double SC2470Processor::getReferenceDac() const
{
    return std::stod(m_details->connection->transact(MessageBuilder::buildQuery(refDac, m_details->slotId)));
}

void SC2470Processor::setReferenceDac(uint32_t dacValue) const
{
    std::stringstream ss;
    ss << "0x" << std::hex << dacValue;
    std::string formatedValue = ss.str();
    std::vector<std::string> params{formatedValue};
    m_details->connection->transact(MessageBuilder::buildCommand(refDac, m_details->slotId, params));
}

SC2470Processor::ReferenceConfig SC2470Processor::getReferenceConfig() const
{
    SC2470Processor::ReferenceConfig returnValues;
    std::vector<std::string> splitResult;
    bool setFlag = false;

    std::string result = m_details->connection->transact(MessageBuilder::buildQuery(refConfig, m_details->slotId));

    boost::split(splitResult, result, boost::is_any_of(" "));

    if (splitResult.size() < 2) throwCommsError();

    setFlag = false;
    for (const auto& [key, value] : ClockSourceStringMap)
    {
        // first string should be clock source
        if (splitResult[0].compare(value) == 0)
        {
            returnValues.clkSource = key;
            setFlag = true;
            break;
        }
    }

    if (!setFlag) throwCommsError();

    setFlag = false;
    for (const auto& [key, value] : ReferenceFreqKhzMap)
    {
        // second string should be frequency
        if (utility::isAlmostEqual(std::stod(splitResult[1]), value))
        {
            returnValues.freqSel = key;
            setFlag = true;
            break;
        }
    }

    if (!setFlag) throwCommsError();

    return returnValues;
}

void SC2470Processor::setReferenceConfig(const SC2470Processor::ReferenceConfig& config) const
{
    std::vector<std::string> params{ClockSourceStringMap.at(config.clkSource)};
    double frequencyKhz;

    // Clock must be 100000 when clock sournce is Internal
    if (config.clkSource == SC2470Processor::ClockSource::Internal) frequencyKhz = ReferenceFreqKhzMap.at(SC2470Processor::ReferenceFreq::Freq100MHz);
    else frequencyKhz = ReferenceFreqKhzMap.at(config.freqSel);
    params.push_back(std::to_string(frequencyKhz));

    m_details->connection->transact(MessageBuilder::buildCommand(refConfig, m_details->slotId, params));
}

double SC2470Processor::getReferenceLockDetect() const
{
    return std::stod(m_details->connection->transact(MessageBuilder::buildQuery(refLockDetect, m_details->slotId)));
    // TODO Come back to this, am i returning 1 or 2 values
}

bool SC2470Processor::getReferenceOutputEnable() const
{
    std::string result = m_details->connection->transact(MessageBuilder::buildQuery(refOutputEnable, m_details->slotId));

    for (const auto& [key, value] : EnableStringMap)
    {
        if (result.compare(value) == 0)
        {
            return key;
        }
    }

    throwCommsError();
    // Never gets here, this is to fix warning
    return false;
}

void SC2470Processor::setReferenceOutputEnable(bool enable) const
{
    std::vector<std::string> params{EnableStringMap.at(enable)};
    m_details->connection->transact(MessageBuilder::buildCommand(refOutputEnable, m_details->slotId, params));
}

bool SC2470Processor::getForceFractionalMode(SC2470::Path path) const
{
    std::string result = m_details->connection->transact(MessageBuilder::buildQuery(synthFractionalMode, m_details->slotId, path));

    if (result == "0") return false;
    else if (result == "1") return true;

    throwCommsError();
    // Never gets here, this is to fix warning
    return false;
}

void SC2470Processor::setForceFractionalMode(SC2470::Path path, bool enable) const
{
    std::vector<std::string> params{};
    if (enable) params.push_back("1");
    else params.push_back("0");
    m_details->connection->transact(MessageBuilder::buildCommand(synthFractionalMode, m_details->slotId, path, params));
}

void SC2470Processor::incrementPhase(SC2470::Path path, double increment) const
{
    std::vector<std::string> params{std::to_string(increment)};
    m_details->connection->transact(MessageBuilder::buildCommand(phaseIncrement, m_details->slotId, path, params));
}

double SC2470Processor::getPhaseAccumulator(SC2470::Path path) const
{
    return std::stod(m_details->connection->transact(MessageBuilder::buildQuery(phaseAccumulator, m_details->slotId, path)));
}

void SC2470Processor::setPhaseAccumulator(SC2470::Path path, double phase) const
{
    std::vector<std::string> params{std::to_string(phase)};
    m_details->connection->transact(MessageBuilder::buildCommand(phaseAccumulator, m_details->slotId, path, params));
}

SC2470Processor::SynthesizerEnableSet SC2470Processor::getSynthEnable(SC2470::Path path) const
{
    std::vector<std::string> splitResult;
    std::string result = m_details->connection->transact(MessageBuilder::buildQuery(synthEnable, m_details->slotId, path));
    boost::split(splitResult, result, boost::is_any_of(" "));

    if (splitResult.size() == 2)
        return {(splitResult.at(0).compare(EnableStringMap.at(true)) == 0), (splitResult.at(1).compare(EnableStringMap.at(true)) == 0)};

    throwCommsError();
    // Never gets here, this is to fix warning
    return {false, false};
}

void SC2470Processor::setSynthEnable(SC2470::Path path, SC2470Processor::SynthesizerEnableSet enables) const
{
    std::vector<std::string> params{EnableStringMap.at(enables.enable1x), EnableStringMap.at(enables.enable2x)};
    m_details->connection->transact(MessageBuilder::buildCommand(synthEnable, m_details->slotId, path, params));
}

SC2470Processor::SynthesizerRfRegisters SC2470Processor::getSynthRfSet(SC2470::Path path) const
{
    SynthesizerRfRegisters result;
    std::vector<std::string> splitResult;
    std::string response = m_details->connection->transact(MessageBuilder::buildQuery(synthRfSet, m_details->slotId, path));
    boost::split(splitResult, response, boost::is_any_of(" "));

    if (splitResult.size() == 5)
    {
        result.intDivider = std::stoi(splitResult.at(0));
        result.frac1 = std::stoul(splitResult.at(1));
        result.frac2 = std::stoul(splitResult.at(2));
        result.mod2 = std::stoul(splitResult.at(3));
        result.rfDivider = std::stoi(splitResult.at(4));
        return result;
    }

    throwCommsError();
    // Never gets here, this is to fix warning
    return result;
}

void SC2470Processor::configApplyToSystem(void) const
{
    m_details->connection->transact(MessageBuilder::buildCommand(configApply, m_details->slotId));
}

void SC2470Processor::configLoadFromDefault(void) const
{
    m_details->connection->transact(MessageBuilder::buildCommand(configDefault, m_details->slotId));
}

void SC2470Processor::configLoadFromNvm(void) const
{
    m_details->connection->transact(MessageBuilder::buildCommand(configLoad, m_details->slotId));
}

void SC2470Processor::configSavetoNvm(void) const
{
    m_details->connection->transact(MessageBuilder::buildCommand(configSave, m_details->slotId));
}

bool SC2470Processor::getConfigAutoload(void) const
{
    std::string autoLoad = m_details->connection->transact(MessageBuilder::buildQuery(configAutoLoad, m_details->slotId));

    if (autoLoad.compare("0") == 0) return false;
    if (autoLoad.compare("1") == 0) return true;

    throwCommsError();
    // Never gets here, this is to fix warning
    return false;
}

void SC2470Processor::setConfigAutoload(bool enable) const
{
    std::vector<std::string> params{};

    if (enable) params.push_back("1");
    else params.push_back("0");

    m_details->connection->transact(MessageBuilder::buildCommand(configAutoLoad, m_details->slotId, params));
}

bool SC2470Processor::getConfigAutoPhase(void) const
{
    std::string autoPhase = m_details->connection->transact(MessageBuilder::buildQuery(configAutoPhase, m_details->slotId));

    if (autoPhase.compare("0") == 0) return false;
    if (autoPhase.compare("1") == 0) return true;

    throwCommsError();
    // Never gets here, this is to fix warning
    return false;
}

void SC2470Processor::setConfigAutoPhase(bool enable) const
{
    std::vector<std::string> params{};

    if (enable) params.push_back("1");
    else params.push_back("0");

    m_details->connection->transact(MessageBuilder::buildCommand(configAutoPhase, m_details->slotId, params));
}

bool SC2470Processor::getLoClkEnable(SC2470::Path path) const
{
    std::string result = m_details->connection->transact(MessageBuilder::buildQuery(loClkEn, m_details->slotId, path));

    if (result.compare(EnableStringMap.at(true)) == 0) return true;
    else if (result.compare(EnableStringMap.at(false)) == 0) return false;

    throwCommsError();
    // Never gets here, this is to fix warning
    return false;
}

void SC2470Processor::setLoClkEnable(SC2470::Path path, bool enable) const
{
    std::vector<std::string> params{EnableStringMap.at(enable)};
    m_details->connection->transact(MessageBuilder::buildCommand(loClkEn, m_details->slotId, path, params));
}

void SC2470Processor::setDCBias(SC2470::Path path, SC2470::DCBias bias) const {
    const std::vector<std::string> params{std::to_string(bias.i), std::to_string(bias.q)};
    m_details->connection->transact(MessageBuilder::buildCommand(ifPathDCBias, m_details->slotId, path, params));
}

SC2470::DCBias SC2470Processor::getDCBias(SC2470::Path path) const {
    SC2470::DCBias resultBias;
    std::vector<std::string> splitResult;

    const std::string result = m_details->connection->transact(MessageBuilder::buildQuery(ifPathDCBias, m_details->slotId, path));

    boost::split(splitResult, result, boost::is_any_of(" "));

    resultBias.i = std::stoi(splitResult[0]);
    resultBias.q = std::stoi(splitResult[1]);

    return resultBias;
}

} // namespace fesd
