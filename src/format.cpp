#include "format.h"

#include <sstream>
#include <string>

std::string Format::NumberToString(const long number) {
  std::stringstream stream;

  if (number < 10) stream << "0";

  stream << number;

  return stream.str();
}

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
std::string Format::ElapsedTime(long seconds) {
  int kseconds_per_hour = 60 * 60;
  int kseconds_per_minute = 60;

  long hours = seconds / (kseconds_per_hour);
  long remainingSeconds = seconds % (kseconds_per_hour);

  long minutes = remainingSeconds / kseconds_per_minute;
  remainingSeconds = seconds % kseconds_per_minute;

  std::stringstream stream;
  stream << NumberToString(hours) << ":" << NumberToString(minutes) << ":"
     << NumberToString(remainingSeconds);

  return stream.str();
}