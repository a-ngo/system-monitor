#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string NumberToString(const long number);
std::string ElapsedTime(long times);
};  // namespace Format

#endif