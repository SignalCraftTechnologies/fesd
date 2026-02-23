#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <fesd/fesd.hpp>
#include "Utility.hpp"

// pybind11-stubgen can be used to generate stubs (.pyi) for the pyfesd output
// https://github.com/sizmailov/pybind11-stubgen
// The order of the types in this file can impact stub creation:
// https://github.com/pybind/pybind11/issues/2350#issuecomment-668032571

namespace py = pybind11;
using namespace py::literals;


PYBIND11_MODULE(pyfesd, module) {
    module.def("version", &fesd::getVersion);

    py::enum_<fesd::SystemRole>(module, "SytemRole")
        .value("Controller", fesd::SystemRole::Controller)
        .value("Peripheral", fesd::SystemRole::Peripheral);
    
    py::enum_<fesd::DeviceType>(module, "DeviceType")
        .value("Undefined", fesd::DeviceType::Undefined)
        .value("SC2470", fesd::DeviceType::SC2470);

    py::enum_<fesd::SC2470::Path>(module, "SC2470Path")
        .value("RX", fesd::SC2470::Path::RX)
        .value("TX", fesd::SC2470::Path::TX);

    py::enum_<fesd::SC2470::ReferenceSource>(module, "SC2470ReferenceSource")
        .value("Internal", fesd::SC2470::ReferenceSource::Internal)
        .value("External10MHz", fesd::SC2470::ReferenceSource::External10MHz)
        .value("External100MHz", fesd::SC2470::ReferenceSource::External100MHz);

    py::enum_<fesd::SC2470::InternalReferenceFrequency>(module, "SC2470InternalReferenceFrequency")
        .value("Automatic", fesd::SC2470::InternalReferenceFrequency::Automatic)
        .value("Force100MHz", fesd::SC2470::InternalReferenceFrequency::Force100MHz)
        .value("Force105MHz", fesd::SC2470::InternalReferenceFrequency::Force105MHz);

    py::enum_<fesd::SC2470::DuplexSetting>(module, "SC2470DuplexSetting")
        .value("Fdd", fesd::SC2470::DuplexSetting::Fdd)
        .value("TddRx", fesd::SC2470::DuplexSetting::TddRx)
        .value("TddTx", fesd::SC2470::DuplexSetting::TddTx);

    py::enum_<fesd::SC2470::SynthesizerMode>(module, "SC2470SynthesizerMode")
        .value("Integer", fesd::SC2470::SynthesizerMode::Integer)
        .value("Fractional", fesd::SC2470::SynthesizerMode::Fractional);

    py::class_<fesd::FEDevice>(module, "FEDevice")
        .def_readonly("slotId", &fesd::FEDevice::slotId)
        .def_readonly("type", &fesd::FEDevice::type)
        .def_readonly("serialNumber", &fesd::FEDevice::serialNumberStr)
        .def_readonly("firmwareVersion", &fesd::FEDevice::firmwareVersion)
        .def_readonly("hardwareVersion", &fesd::FEDevice::hardwareVersion);

    py::class_<fesd::SC2470::SynthesizerSettings>(module, "SC2470SynthesizerSettings")
        .def(py::init<bool, bool, uint16_t, uint16_t>(), "enable1xInput"_a, "enable2xInput"_a, "power1xInput"_a, "power2xInput"_a);

    py::class_<fesd::SC2470::RfFrequency>(module, "SC2470RfFrequency")
        .def(py::init<double>(), "Hz"_a);

    py::class_<fesd::SC2470::IfFrequency>(module, "SC2470IfFrequency")
        .def(py::init<double>(), "Hz"_a);

    py::class_<fesd::SC2470::LoFrequency>(module, "SC2470LoFrequency")
        .def(py::init<double>(), "Hz"_a);

    py::class_<fesd::SC2470::BypassFrequency>(module, "SC2470BypassFrequency")
        .def(py::init<double>(), "Hz"_a);
    
    py::class_<fesd::SC2470::DCBias>(module, "SC2470DCBias")
        .def(py::init<int16_t, int16_t>(), "i"_a, "q"_a)
        .def_readonly("i", &fesd::SC2470::DCBias::i)
        .def_readonly("q", &fesd::SC2470::DCBias::q);

    py::class_<fesd::SC2470::FrequencySet>(module, "SC2470FrequencySet")
        .def(py::init<double, double, double>(), "rfHz"_a, "ifHz"_a, "loHz"_a)
        .def_readonly("rfHz", &fesd::SC2470::FrequencySet::rfHz)
        .def_readonly("ifHz", &fesd::SC2470::FrequencySet::ifHz)
        .def_readonly("loHz", &fesd::SC2470::FrequencySet::loHz);

    py::class_<fesd::SC2470::GainLimitsSet>(module, "SC2470GainLimitsSet")
        .def(py::init<double, double>(), "minDb"_a, "maxDb"_a)
        .def_readonly("minDb", &fesd::SC2470::GainLimitsSet::minDb)
        .def_readonly("maxDb", &fesd::SC2470::GainLimitsSet::maxDb);
    
    py::class_<fesd::SC2470Commander>(module, "SC2470Commander")
        .def("resetDevice", &fesd::SC2470Commander::resetDevice)
        .def("getId", &fesd::SC2470Commander::getSlotId)
        .def("getDeviceType", &fesd::SC2470Commander::getDeviceType)
        .def("getSystemRole", &fesd::SC2470Commander::getSystemRole)
        .def("getFirmwareVersion", &fesd::SC2470Commander::getFirmwareVersion)
        .def("getSystemRole", &fesd::SC2470Commander::getSystemRole)
        .def("getSerialNumber", &fesd::SC2470Commander::getSerialNumber)
        .def("getGainLimits", &fesd::SC2470Commander::getGainLimits, "path"_a)
        .def("getGain", &fesd::SC2470Commander::getGain, "path"_a)
        .def("configureGain", &fesd::SC2470Commander::configureGain, "path"_a, "gainDb"_a)
        .def("getAttenuation", &fesd::SC2470Commander::getAttenuation, "path"_a)
        .def("configureAttenuation", &fesd::SC2470Commander::configureAttenuation, "path"_a, "attenuationDb"_a)
        .def("getGain", &fesd::SC2470Commander::getGain, "path"_a)
        .def("getFrequencies", &fesd::SC2470Commander::getFrequencies, "path"_a)
        .def("configureFrequencies", py::overload_cast<fesd::SC2470::Path, fesd::SC2470::RfFrequency, fesd::SC2470::IfFrequency>(&fesd::SC2470Commander::configureFrequencies, py::const_), "path"_a, "rfFreq"_a, "ifFreq"_a)
        .def("configureFrequencies", py::overload_cast<fesd::SC2470::Path, fesd::SC2470::RfFrequency, fesd::SC2470::LoFrequency>(&fesd::SC2470Commander::configureFrequencies, py::const_), "path"_a, "rfFreq"_a, "loFreq"_a)
        .def("configureFrequencies", py::overload_cast<fesd::SC2470::Path, fesd::SC2470::IfFrequency, fesd::SC2470::LoFrequency>(&fesd::SC2470Commander::configureFrequencies, py::const_), "path"_a, "ifFreq"_a, "loFreq"_a)
        .def("configureFrequencies", py::overload_cast<fesd::SC2470::Path, fesd::SC2470::FrequencySet>(&fesd::SC2470Commander::configureFrequencies, py::const_), "path"_a, "freqs"_a)
        .def("configureBypassFrequency", &fesd::SC2470Commander::configureBypassFrequency, "path"_a, "byFreq"_a)
        .def("getSynthesizerSettings", &fesd::SC2470Commander::getSynthesizerSettings, "path"_a)
        .def("configureSynthesizerSettings", &fesd::SC2470Commander::configureSynthesizerSettings, "path"_a, "settings"_a)
        .def("getLoEnable", &fesd::SC2470Commander::getLoEnable, "path"_a)
        .def("configureLoEnable", &fesd::SC2470Commander::configureLoEnable, "path"_a, "enable"_a)
        .def("getLoFrequency", &fesd::SC2470Commander::getLoFrequency, "path"_a)
        .def("configureLoFrequency", &fesd::SC2470Commander::configureLoFrequency, "path"_a, "frequencyHz"_a)
        .def("getDuplexSetting", &fesd::SC2470Commander::getDuplexSetting)
        .def("configureDuplexSetting", &fesd::SC2470Commander::configureDuplexSetting, "setting"_a) 
        .def("getReferenceSource", &fesd::SC2470Commander::getReferenceSource)        
        .def("configureReferenceSource", &fesd::SC2470Commander::configureReferenceSource, "source"_a)
        .def("getInternalReferenceOverride", &fesd::SC2470Commander::getInternalReferenceOverride, "path"_a)        
        .def("configureInternalReferenceOverride", &fesd::SC2470Commander::configureInternalReferenceOverride, "path"_a, "freq"_a)
        .def("getPhaseOffset", &fesd::SC2470Commander::getPhaseOffset, "path"_a)
        .def("configurePhaseOffset", &fesd::SC2470Commander::configurePhaseOffset, "path"_a, "offset"_a)
        .def("getSynthesizerMode", &fesd::SC2470Commander::getSynthesizerMode, "path"_a)
        .def("configureDCBias", &fesd::SC2470Commander::configureDCBias, "path"_a, "bias"_a)
        .def("getDCBias", &fesd::SC2470Commander::getDCBias, "path"_a)
        .def("configureReferenceOutputEnable", &fesd::SC2470Commander::configureReferenceOutputEnable, "enable"_a)
        .def("getReferenceOutputEnable", &fesd::SC2470Commander::getReferenceOutputEnable);


    py::class_<fesd::FESerialDriver>(module, "FESerialDriver")
        .def(py::init<const std::string &>(), "ports"_a)
        .def("getDevices", &fesd::FESerialDriver::getDevices)
        .def("getSC2470Commander", py::overload_cast<const std::string&>(&fesd::FESerialDriver::getSC2470Commander, py::const_), "serialNumber"_a)
        .def("getSC2470Commander", py::overload_cast<const uint16_t>(&fesd::FESerialDriver::getSC2470Commander, py::const_), "slotId"_a)
        .def("getSC2470Commander", py::overload_cast<const fesd::FEDevice&>(&fesd::FESerialDriver::getSC2470Commander, py::const_), "device"_a)
        .def("getSC2470Commanders", &fesd::FESerialDriver::getSC2470Commanders);
}