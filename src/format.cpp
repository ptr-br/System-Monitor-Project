#include "format.h"

#include <iomanip>
#include <iostream>
#include <string>

#define HOUR 3600
#define MINUTE 60

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long up_time) {
  int hours;
  int minutes;
  int seconds;
  int secondsRemaining;

  hours = up_time / HOUR;
  secondsRemaining = up_time % HOUR;
  minutes = secondsRemaining / MINUTE;
  seconds = secondsRemaining % MINUTE;

  std::ostringstream oss;

  oss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2)
      << std::setfill('0') << minutes << ":" << std::setw(2)
      << std::setfill('0') << seconds;

  return oss.str();
}
