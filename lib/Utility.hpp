#pragma once

#include <limits>

namespace fesd 
{

namespace utility
{

bool isAlmostEqual(const double a, const double b, const double epsilon=std::numeric_limits<double>::epsilon());
bool isAlmostEqualToZero(const double a, const double epsilon);

} // namespace utility

} // namespace fesd