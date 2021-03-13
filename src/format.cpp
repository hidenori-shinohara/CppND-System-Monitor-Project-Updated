#include <cassert>
#include <sstream>
#include <string>

#include "format.h"

using std::string;

#include <iostream>
// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hours = seconds / 3600;
  seconds %= 3600;
  long minutes = seconds / 60;
  seconds %= 60;
  std::stringstream ss;
  if (hours < 10) {
    ss << "0";
  }
  ss << hours << ":";
  if (minutes < 10) {
    ss << "0";
  }
  ss << minutes << ":";
  if (seconds < 10) {
    ss << "0";
  }
  ss << seconds;
  return ss.str();
}