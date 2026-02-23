#pragma once

#include <cstdint>

namespace fesd 
{

namespace SC2470
{

enum class ReferenceSource
{
    Internal,
    External10MHz,
    External100MHz,
};

enum class InternalReferenceFrequency
{
    Automatic,
    Force100MHz,
    Force105MHz,
};

enum class DuplexSetting
{
    Fdd,
    TddRx,
    TddTx,
};

enum class SynthesizerMode
{
    Integer,
    Fractional,
};

enum class Path
{
    RX,
    TX,
};

struct SynthesizerSettings
{
    static constexpr uint16_t powerMax = 15;
    bool enable1x;
    bool enable2x;
    uint16_t powerLevel1x;
    uint16_t powerLevel2x;

    SynthesizerSettings(bool enable1xInput, bool enable2xInput, uint16_t power1xInput, uint16_t power2xInput)
    {
        enable1x = enable1xInput;
        enable2x = enable2xInput;
        powerLevel1x = power1xInput;
        powerLevel2x = power2xInput;

        if (powerLevel1x > powerMax)
            powerLevel1x = powerMax;

        if (powerLevel2x > powerMax)
            powerLevel2x = powerMax;
    }
};

struct RfFrequency
{
    static constexpr double rfMaxHz = 26E9;
    static constexpr double rfMinHz = 6.7E9;
    double rfHz;

    RfFrequency(void) {}
    RfFrequency(double input)
    {
        if (input < rfMinHz)
            input = rfMinHz;
        if (input > rfMaxHz)
            input = rfMaxHz;
        rfHz = input;
    }

    RfFrequency& operator=(const RfFrequency& newVals)
    {
        rfHz = newVals.rfHz;
        return *this;
    }
};

struct IfFrequency
{
    static constexpr double ifMaxHz = 6E9;
    static constexpr double ifMinHz = 700E6;
    double ifHz;

    IfFrequency(void) {}
    IfFrequency(double input)
    {
        if (input < ifMinHz)
            input = ifMinHz;
        if (input > ifMaxHz)
            input = ifMaxHz;
        ifHz = input;
    }

    IfFrequency& operator=(const IfFrequency& newVals)
    {
        ifHz = newVals.ifHz;
        return *this;
    }
};

struct LoFrequency
{
    static constexpr double loMaxHz = 25.3E9;
    static constexpr double loMinHz = 6E9;
    double loHz;

    LoFrequency(void) {}
    LoFrequency(double input)
    {
        if (input < loMinHz)
            input = loMinHz;
        if (input > loMaxHz)
            input = loMaxHz;
        loHz = input;
    }

    LoFrequency& operator=(const LoFrequency& newVals)
    {
        loHz = newVals.loHz;
        return *this;
    }
};

struct BypassFrequency
{
    static constexpr double byMaxHz = 8.5E9;
    static constexpr double byMinHz = 50E6;
    double byHz;

    BypassFrequency(void) {}
    BypassFrequency(double input)
    {
        if (input < byMinHz)
            input = byMinHz;
        if (input > byMaxHz)
            input = byMaxHz;
        byHz = input;
    }

    BypassFrequency& operator=(const BypassFrequency& newVals)
    {
        byHz = newVals.byHz;
        return *this;
    }
};

struct FrequencySet
{
    double rfHz;
    double ifHz;
    double loHz; 
};

struct GainLimitsSet
{
    double minDb;
    double maxDb;

    GainLimitsSet& operator=(const GainLimitsSet& newVals)
    {
        minDb = newVals.minDb;
        maxDb = newVals.maxDb;
        return *this;
    }

};

struct DCBias
{
    static constexpr int16_t biasMin = -2048;
    static constexpr int16_t biasMax = 2047;
    int i;
    int q;

    DCBias(void) {}
    DCBias(int16_t inputI, int16_t inputQ)
    {
        if (inputI < biasMin)
            inputI = biasMin;
        if (inputI > biasMax)
            inputI = biasMax;
        if (inputQ < biasMin)
            inputQ = biasMin;
        if (inputQ > biasMax)
            inputQ = biasMax;
        i = inputI;
        q = inputQ;
    }

    DCBias& operator=(const DCBias& newVals)
    {
        i = newVals.i;
        q = newVals.q;
        return *this;
    }

};

} // namespace SC2470

} // namespace fesd