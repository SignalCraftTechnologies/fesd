#pragma once

#include <stdexcept>

namespace fesd {

class CommunicationError : public std::runtime_error
{
public:
    CommunicationError(const std::string& what) : std::runtime_error(what) {}
};

class InvalidArgumentsError : public std::runtime_error
{
public:
    InvalidArgumentsError(const std::string& what) : std::runtime_error(what) {}
};

class CalibrationError : public std::runtime_error
{
public:
    CalibrationError(const std::string& what) : std::runtime_error(what) {}
};

} // namespace fesd